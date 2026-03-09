/*
  cpp-playground - C++ experiments and learning playground
  Copyright (C) 2025 M. Reza Dwi Prasetiawan

  GNU GPL v3 — see <https://www.gnu.org/licenses/>

  ─────────────────────────────────────────────────────────────────────────────
  Scene objects:
    • Cube        – rotating, centre, orange
    • Cube small  – static, kiri belakang, teal
    • Cube tall   – static, kanan, merah
    • Pyramid     – static, kanan depan, hijau
    • Sphere      – static, kiri, ungu  (UV 24×16)
    • Floor plane – static, y = -1.1, abu terang

  Lighting:
    • Satu point-light di (0,0,0) → omnidirectional secara alami
    • Ambient 0.20, diffuse 0.75, specular 0.35 → tidak terlalu terang
    • PCF soft shadow dari ortho light atas (approx omni shadow)

  Pipeline (6 passes):
    Pass 1 │ Shadow Map    – semua objek, depth-only (2048²)
    Pass 2 │ Scene → HDR  – Blinn-Phong omni + PCF shadow, velocity
    Pass 3 │ Bloom         – 10× Gaussian ping-pong
    Pass 4 │ Motion Blur   – velocity warp 12 samples
    Pass 5 │ Composite     – Reinhard + bloom + γ
    Pass 6 │ Outer cube    – flat unlit putih+grid (depth blit, no tonemap)
  ─────────────────────────────────────────────────────────────────────────────
*/

#pragma once
#include <QMatrix4x4>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QQuaternion>
#include <QTimer>
#include <cmath>
#include <vector>

class GLwidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {
  Q_OBJECT

 public:
  explicit GLwidget(QWidget* parent = nullptr) : QOpenGLWidget(parent) {}

 protected:
  // ── Geometry VAOs ─────────────────────────────────────────────────────────
  GLuint cubeVAO    = 0, cubeVBO    = 0, cubeEBO    = 0;
  GLuint pyramidVAO = 0, pyramidVBO = 0, pyramidEBO = 0;
  GLuint sphereVAO  = 0, sphereVBO  = 0, sphereEBO  = 0;
  GLuint planeVAO   = 0, planeVBO   = 0, planeEBO   = 0;
  GLuint skyVAO     = 0, skyVBO     = 0, skyEBO     = 0;
  GLuint quadVAO    = 0, quadVBO    = 0, quadEBO    = 0;
  GLsizei sphereIdxCount = 0;
  GLsizei pyramidIdxCount = 0;

  // ── Shaders ───────────────────────────────────────────────────────────────
  QOpenGLShaderProgram shadowSP, mainSP, blurSP, motionSP, compositeSP, flatSP;

  // ── FBOs / Textures ───────────────────────────────────────────────────────
  GLuint shadowFBO = 0, shadowDepth = 0;
  static constexpr int SHADOW_SZ = 2048;

  GLuint hdrFBO = 0, hdrColor = 0, hdrBright = 0, hdrVelocity = 0, hdrDepthTex = 0;
  GLuint bloomFBO[2] = {}, bloomTex[2] = {};
  GLuint motionFBO = 0, motionTex = 0;

  // ── State ─────────────────────────────────────────────────────────────────
  QMatrix4x4  view, proj;
  QMatrix4x4  prevRotMVP;                 // hanya kubus utama yang bergerak
  QMatrix4x4  lightView, lightProj;
  QQuaternion rotation;
  int W = 800, H = 600;

  // ══════════════════════════════════════════════════════════════════════════
  //  SHADERS
  // ══════════════════════════════════════════════════════════════════════════

  // Pass 1 – shadow depth
  static constexpr const char* kShadowVS = R"(#version 330 core
  layout(location=0) in vec3 aPos;
  uniform mat4 lightMVP;
  void main(){ gl_Position = lightMVP * vec4(aPos,1.0); })";
  static constexpr const char* kShadowFS = R"(#version 330 core
  void main(){})";

  // Pass 2 – scene (Blinn-Phong omni, PCF shadow, velocity)
  static constexpr const char* kMainVS = R"(#version 330 core
  layout(location=0) in vec3 aPos;
  layout(location=1) in vec3 aNormal;
  out vec3 vPos; out vec3 vNorm;
  out vec4 vLsPos; out vec4 vCurr; out vec4 vPrev;
  uniform mat4 model, view, proj, lightSpace, prevMVP;
  void main(){
    vec4 w  = model * vec4(aPos,1.0);
    vPos    = w.xyz;
    vNorm   = mat3(transpose(inverse(model))) * aNormal;
    vLsPos  = lightSpace * w;
    vCurr   = proj * view * w;
    vPrev   = prevMVP * vec4(aPos,1.0);
    gl_Position = vCurr;
  })";

  static constexpr const char* kMainFS = R"(#version 330 core
  in vec3 vPos; in vec3 vNorm;
  in vec4 vLsPos; in vec4 vCurr; in vec4 vPrev;
  layout(location=0) out vec4 oColor;
  layout(location=1) out vec4 oBright;
  layout(location=2) out vec2 oVel;
  uniform vec3 ViewPos, ObjectCol;
  uniform sampler2D shadowMap;

  // PCF 5×5 soft shadow
  float pcf(vec4 ls, vec3 n, vec3 l){
    vec3 p = ls.xyz/ls.w*0.5+0.5;
    if(p.z>1.0) return 0.0;
    float b=max(0.005*(1.0-dot(n,l)),0.001);
    float s=0.0; vec2 ts=1.0/textureSize(shadowMap,0);
    for(int x=-2;x<=2;x++) for(int y=-2;y<=2;y++){
      float d=texture(shadowMap,p.xy+vec2(x,y)*ts).r;
      s+=(p.z-b>d)?1.0:0.0;
    }
    return s/25.0;
  }

  void main(){
    vec3 n = normalize(vNorm);
    vec3 v = normalize(ViewPos - vPos);

    // Satu point-light di origin → omnidirectional
    vec3  l    = normalize(-vPos);   // arah fragment → origin
    vec3  h    = normalize(l + v);
    float diff = max(dot(n,l), 0.0);
    float spec = pow(max(dot(n,h), 0.0), 64.0);
    float sh   = pcf(vLsPos, n, l);

    // Redup: ambient 0.20, diffuse 0.75, specular 0.35
    float lit  = 0.20 + (1.0-sh*0.5)*(diff*0.75 + spec*0.35);
    vec3 result = clamp(lit,0.0,1.5) * ObjectCol;

    oColor  = vec4(result, 1.0);
    float lum = dot(result, vec3(0.2126,0.7152,0.0722));
    oBright = (lum > 0.80) ? vec4(result,1.0) : vec4(0.0);
    oVel = vCurr.xy/vCurr.w*0.5+0.5 - vPrev.xy/vPrev.w*0.5-0.5;
  })";

  // Pass 3 – blur
  static constexpr const char* kBlurVS = R"(#version 330 core
  layout(location=0) in vec2 aPos; layout(location=1) in vec2 aUV;
  out vec2 vUV; void main(){ vUV=aUV; gl_Position=vec4(aPos,0,1); })";
  static constexpr const char* kBlurFS = R"(#version 330 core
  in vec2 vUV; out vec4 oColor;
  uniform sampler2D image; uniform bool horizontal;
  const float w[5]=float[](0.227027,0.194595,0.121622,0.054054,0.016216);
  void main(){
    vec2 d=1.0/textureSize(image,0);
    vec2 dir=horizontal?vec2(d.x,0):vec2(0,d.y);
    vec3 c=texture(image,vUV).rgb*w[0];
    for(int i=1;i<5;i++){c+=texture(image,vUV+dir*i).rgb*w[i];
                          c+=texture(image,vUV-dir*i).rgb*w[i];}
    oColor=vec4(c,1.0);
  })";

  // Pass 4 – motion blur
  static constexpr const char* kMotionVS = R"(#version 330 core
  layout(location=0) in vec2 aPos; layout(location=1) in vec2 aUV;
  out vec2 vUV; void main(){ vUV=aUV; gl_Position=vec4(aPos,0,1); })";
  static constexpr const char* kMotionFS = R"(#version 330 core
  in vec2 vUV; out vec4 oColor;
  uniform sampler2D scene, velocityMap;
  void main(){
    vec2 vel=texture(velocityMap,vUV).rg*0.30;
    vec3 c=texture(scene,vUV).rgb;
    for(int i=1;i<12;i++) c+=texture(scene,vUV-vel*(float(i)/11.0)).rgb;
    oColor=vec4(c/12.0,1.0);
  })";

  // Pass 5 – composite (Reinhard + bloom + gamma)
  static constexpr const char* kCompVS = R"(#version 330 core
  layout(location=0) in vec2 aPos; layout(location=1) in vec2 aUV;
  out vec2 vUV; void main(){ vUV=aUV; gl_Position=vec4(aPos,0,1); })";
  static constexpr const char* kCompFS = R"(#version 330 core
  in vec2 vUV; out vec4 oColor;
  uniform sampler2D scene, bloom; uniform float exposure;
  void main(){
    vec3 c=texture(scene,vUV).rgb+texture(bloom,vUV).rgb*0.7;
    c=vec3(1.0)-exp(-c*exposure);
    c=pow(c,vec3(1.0/2.2));
    oColor=vec4(c,1.0);
  })";

  // Pass 6 – outer cube flat (putih + grid)
  static constexpr const char* kFlatVS = R"(#version 330 core
  layout(location=0) in vec3 aPos;
  out vec3 vPos;
  uniform mat4 view, proj;
  void main(){
    vec3 w=aPos*500.0; vPos=w;
    gl_Position=proj*view*vec4(w,1.0);
  })";
  static constexpr const char* kFlatFS = R"(#version 330 core
  in vec3 vPos; out vec4 oColor;
  void main(){
    vec3  gf   = abs(fract(vPos*0.02+0.5)-0.5);
    float line = 1.0-smoothstep(0.0,0.04,min(gf.x,min(gf.y,gf.z)));
    oColor=vec4(mix(vec3(0.96),vec3(0.60,0.60,0.63),line*0.85),1.0);
  })";

  // ══════════════════════════════════════════════════════════════════════════
  //  HELPERS
  // ══════════════════════════════════════════════════════════════════════════
  void compileSP(QOpenGLShaderProgram& sp, const char* vs, const char* fs){
    sp.addShaderFromSourceCode(QOpenGLShader::Vertex,   vs);
    sp.addShaderFromSourceCode(QOpenGLShader::Fragment, fs);
    sp.link();
  }

  GLuint makeTex(GLenum iF, GLenum bF, GLenum ty, int w, int h,
                 GLenum wrap=GL_CLAMP_TO_EDGE, GLenum fil=GL_LINEAR){
    GLuint t; glGenTextures(1,&t);
    glBindTexture(GL_TEXTURE_2D,t);
    glTexImage2D(GL_TEXTURE_2D,0,iF,w,h,0,bF,ty,nullptr);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,fil);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,fil);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,wrap);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,wrap);
    return t;
  }

  // Upload pos(3f)+normal(3f) geometry
  void uploadMesh(GLuint& vao, GLuint& vbo, GLuint& ebo,
                  const std::vector<float>& vd, const std::vector<unsigned int>& id){
    glGenVertexArrays(1,&vao);
    glGenBuffers(1,&vbo); glGenBuffers(1,&ebo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER,vbo);
    glBufferData(GL_ARRAY_BUFFER,vd.size()*4,vd.data(),GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,id.size()*4,id.data(),GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(12));
    glEnableVertexAttribArray(1);
  }

  // Upload pos-only geometry (skybox)
  void uploadPosOnly(GLuint& vao, GLuint& vbo, GLuint& ebo,
                     const std::vector<float>& vd, const std::vector<unsigned int>& id){
    glGenVertexArrays(1,&vao);
    glGenBuffers(1,&vbo); glGenBuffers(1,&ebo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER,vbo);
    glBufferData(GL_ARRAY_BUFFER,vd.size()*4,vd.data(),GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,id.size()*4,id.data(),GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
  }

  void bindTex(int unit, GLuint tex){
    glActiveTexture(GL_TEXTURE0+unit);
    glBindTexture(GL_TEXTURE_2D,tex);
  }

  // ── Geometry generators ───────────────────────────────────────────────────

  // Kubus unit  (pos+normal, 24 verts, 36 idx)
  static void makeCube(std::vector<float>& v, std::vector<unsigned int>& idx){
    v = {
      -0.5f,-0.5f,-0.5f, 0,0,-1,  0.5f,-0.5f,-0.5f, 0,0,-1,
       0.5f, 0.5f,-0.5f, 0,0,-1, -0.5f, 0.5f,-0.5f, 0,0,-1,
      -0.5f,-0.5f, 0.5f, 0,0, 1,  0.5f,-0.5f, 0.5f, 0,0, 1,
       0.5f, 0.5f, 0.5f, 0,0, 1, -0.5f, 0.5f, 0.5f, 0,0, 1,
      -0.5f, 0.5f, 0.5f,-1,0, 0, -0.5f, 0.5f,-0.5f,-1,0, 0,
      -0.5f,-0.5f,-0.5f,-1,0, 0, -0.5f,-0.5f, 0.5f,-1,0, 0,
       0.5f, 0.5f, 0.5f, 1,0, 0,  0.5f, 0.5f,-0.5f, 1,0, 0,
       0.5f,-0.5f,-0.5f, 1,0, 0,  0.5f,-0.5f, 0.5f, 1,0, 0,
      -0.5f,-0.5f,-0.5f, 0,-1,0,  0.5f,-0.5f,-0.5f, 0,-1,0,
       0.5f,-0.5f, 0.5f, 0,-1,0, -0.5f,-0.5f, 0.5f, 0,-1,0,
      -0.5f, 0.5f,-0.5f, 0, 1,0,  0.5f, 0.5f,-0.5f, 0, 1,0,
       0.5f, 0.5f, 0.5f, 0, 1,0, -0.5f, 0.5f, 0.5f, 0, 1,0,
    };
    idx = {
       0,1,2,2,3,0,  4,5,6,6,7,4,
       8,9,10,10,11,8,  12,13,14,14,15,12,
      16,17,18,18,19,16,  20,21,22,22,23,20,
    };
  }

  // Piramida persegi (base unit 1×1 di y=0, apex y=1)
  static void makePyramid(std::vector<float>& v, std::vector<unsigned int>& idx){
    // 4 sisi segitiga + 1 alas persegi (2 segitiga) = 18 vertex (flat normal)
    auto push3 = [&](float x, float y, float z,
                     float nx, float ny, float nz){
      v.push_back(x);v.push_back(y);v.push_back(z);
      v.push_back(nx);v.push_back(ny);v.push_back(nz);
    };

    // Pre-computed flat normals per sisi
    // Front  (+z), Right (+x), Back (-z), Left (-x)
    float sn = sqrtf(0.2f), sy = sqrtf(0.8f);   // approx slope normal
    // Front face
    push3(-0.5f,0, 0.5f,    0,sy, sn);
    push3( 0.5f,0, 0.5f,    0,sy, sn);
    push3( 0,   1, 0,       0,sy, sn);
    // Right face
    push3( 0.5f,0, 0.5f,   sn,sy, 0);
    push3( 0.5f,0,-0.5f,   sn,sy, 0);
    push3( 0,   1, 0,      sn,sy, 0);
    // Back face
    push3( 0.5f,0,-0.5f,    0,sy,-sn);
    push3(-0.5f,0,-0.5f,    0,sy,-sn);
    push3( 0,   1, 0,       0,sy,-sn);
    // Left face
    push3(-0.5f,0,-0.5f,  -sn,sy, 0);
    push3(-0.5f,0, 0.5f,  -sn,sy, 0);
    push3( 0,   1, 0,     -sn,sy, 0);
    // Base (normal down)
    push3(-0.5f,0,-0.5f,   0,-1,0);
    push3( 0.5f,0,-0.5f,   0,-1,0);
    push3( 0.5f,0, 0.5f,   0,-1,0);
    push3(-0.5f,0,-0.5f,   0,-1,0);
    push3( 0.5f,0, 0.5f,   0,-1,0);
    push3(-0.5f,0, 0.5f,   0,-1,0);

    idx.resize(18); for(int i=0;i<18;i++) idx[i]=i;
  }

  // Sphere UV  (radius 1, stacks, slices)
  static void makeSphere(int stacks, int slices,
                         std::vector<float>& v, std::vector<unsigned int>& idx){
    const float PI = 3.14159265f;
    for(int i=0;i<=stacks;i++){
      float phi = PI * float(i) / float(stacks);
      for(int j=0;j<=slices;j++){
        float theta = 2*PI * float(j) / float(slices);
        float x = sinf(phi)*cosf(theta);
        float y = cosf(phi);
        float z = sinf(phi)*sinf(theta);
        v.push_back(x); v.push_back(y); v.push_back(z);  // pos (radius 1)
        v.push_back(x); v.push_back(y); v.push_back(z);  // normal = pos/r
      }
    }
    for(int i=0;i<stacks;i++) for(int j=0;j<slices;j++){
      unsigned int a = i*(slices+1)+j, b=a+slices+1;
      idx.push_back(a); idx.push_back(b);   idx.push_back(a+1);
      idx.push_back(b); idx.push_back(b+1); idx.push_back(a+1);
    }
  }

  // Floor plane (1×1 unit, normal up)
  static void makePlane(std::vector<float>& v, std::vector<unsigned int>& idx){
    v = {
      -0.5f,0,-0.5f, 0,1,0,
       0.5f,0,-0.5f, 0,1,0,
       0.5f,0, 0.5f, 0,1,0,
      -0.5f,0, 0.5f, 0,1,0,
    };
    idx = {0,1,2, 2,3,0};
  }

  // Skybox cube (pos only, unit ±1)
  static void makeSkybox(std::vector<float>& v, std::vector<unsigned int>& idx){
    v = {
      -1,-1,-1,  1,-1,-1,  1, 1,-1, -1, 1,-1,
      -1,-1, 1,  1,-1, 1,  1, 1, 1, -1, 1, 1,
      -1, 1, 1, -1, 1,-1, -1,-1,-1, -1,-1, 1,
       1, 1, 1,  1, 1,-1,  1,-1,-1,  1,-1, 1,
      -1,-1,-1,  1,-1,-1,  1,-1, 1, -1,-1, 1,
      -1, 1,-1,  1, 1,-1,  1, 1, 1, -1, 1, 1,
    };
    idx = {
       0,1,2,2,3,0,  4,5,6,6,7,4,
       8,9,10,10,11,8,  12,13,14,14,15,12,
      16,17,18,18,19,16,  20,21,22,22,23,20,
    };
  }

  // ══════════════════════════════════════════════════════════════════════════
  //  Draw helpers
  // ══════════════════════════════════════════════════════════════════════════
  void drawMesh(GLuint vao, GLsizei count){
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES,count,GL_UNSIGNED_INT,nullptr);
  }

  // ── Shortcut: set all main-shader object uniforms then draw ──────────────
  void drawObject(GLuint vao, GLsizei count,
                  const QMatrix4x4& mdl, const QMatrix4x4& prevMdlMVP,
                  const QMatrix4x4& lightSpace,
                  const QVector3D& color){
    mainSP.setUniformValue("model",      mdl);
    mainSP.setUniformValue("prevMVP",    prevMdlMVP);
    mainSP.setUniformValue("lightSpace", lightSpace);
    mainSP.setUniformValue("ObjectCol",  color);
    drawMesh(vao, count);
  }

  void shadowObject(GLuint vao, GLsizei count,
                    const QMatrix4x4& lightMVP){
    shadowSP.setUniformValue("lightMVP", lightMVP);
    drawMesh(vao, count);
  }

  // ══════════════════════════════════════════════════════════════════════════
  //  setupHDRFBO
  // ══════════════════════════════════════════════════════════════════════════
  void setupHDRFBO(int w, int h){
    auto dF=[&](GLuint&f){if(f){glDeleteFramebuffers(1,&f);f=0;}};
    auto dT=[&](GLuint&t){if(t){glDeleteTextures(1,&t);t=0;}};
    dF(hdrFBO); dT(hdrColor); dT(hdrBright); dT(hdrVelocity); dT(hdrDepthTex);
    for(int i=0;i<2;i++){dF(bloomFBO[i]);dT(bloomTex[i]);}
    dF(motionFBO); dT(motionTex);

    glGenFramebuffers(1,&hdrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER,hdrFBO);
    hdrColor    = makeTex(GL_RGB16F,GL_RGB,GL_FLOAT,w,h);
    hdrBright   = makeTex(GL_RGB16F,GL_RGB,GL_FLOAT,w,h);
    hdrVelocity = makeTex(GL_RG16F, GL_RG, GL_FLOAT,w,h);
    hdrDepthTex = makeTex(GL_DEPTH_COMPONENT24,GL_DEPTH_COMPONENT,GL_FLOAT,
                          w,h,GL_CLAMP_TO_EDGE,GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,hdrColor,0);
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT1,GL_TEXTURE_2D,hdrBright,0);
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT2,GL_TEXTURE_2D,hdrVelocity,0);
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,hdrDepthTex,0);
    GLenum bufs[]={GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2};
    glDrawBuffers(3,bufs);
    glBindFramebuffer(GL_FRAMEBUFFER,0);

    for(int i=0;i<2;i++){
      glGenFramebuffers(1,&bloomFBO[i]);
      bloomTex[i]=makeTex(GL_RGB16F,GL_RGB,GL_FLOAT,w,h);
      glBindFramebuffer(GL_FRAMEBUFFER,bloomFBO[i]);
      glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,bloomTex[i],0);
    }
    glGenFramebuffers(1,&motionFBO);
    motionTex=makeTex(GL_RGB16F,GL_RGB,GL_FLOAT,w,h);
    glBindFramebuffer(GL_FRAMEBUFFER,motionFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,motionTex,0);
    glBindFramebuffer(GL_FRAMEBUFFER,0);
  }

  // ══════════════════════════════════════════════════════════════════════════
  //  initializeGL
  // ══════════════════════════════════════════════════════════════════════════
  void initializeGL() override {
    initializeOpenGLFunctions();

    compileSP(shadowSP,    kShadowVS, kShadowFS);
    compileSP(mainSP,      kMainVS,   kMainFS);
    compileSP(blurSP,      kBlurVS,   kBlurFS);
    compileSP(motionSP,    kMotionVS, kMotionFS);
    compileSP(compositeSP, kCompVS,   kCompFS);
    compileSP(flatSP,      kFlatVS,   kFlatFS);

    // ── Build and upload all geometry ─────────────────────────────────────
    { std::vector<float> v; std::vector<unsigned int> i;
      makeCube(v,i); uploadMesh(cubeVAO,cubeVBO,cubeEBO,v,i); }

    { std::vector<float> v; std::vector<unsigned int> i;
      makePyramid(v,i); pyramidIdxCount=i.size();
      uploadMesh(pyramidVAO,pyramidVBO,pyramidEBO,v,i); }

    { std::vector<float> v; std::vector<unsigned int> i;
      makeSphere(24,24,v,i); sphereIdxCount=i.size();
      uploadMesh(sphereVAO,sphereVBO,sphereEBO,v,i); }

    { std::vector<float> v; std::vector<unsigned int> i;
      makePlane(v,i); uploadMesh(planeVAO,planeVBO,planeEBO,v,i); }

    { std::vector<float> v; std::vector<unsigned int> i;
      makeSkybox(v,i); uploadPosOnly(skyVAO,skyVBO,skyEBO,v,i); }

    // Screen quad
    static const float qv[]={-1,-1,0,0, 1,-1,1,0, 1,1,1,1, -1,1,0,1};
    static const unsigned int qi[]={0,1,2,2,3,0};
    { GLuint vb,eb;
      glGenVertexArrays(1,&quadVAO); glGenBuffers(1,&vb); glGenBuffers(1,&eb);
      glBindVertexArray(quadVAO);
      glBindBuffer(GL_ARRAY_BUFFER,vb); glBufferData(GL_ARRAY_BUFFER,sizeof(qv),qv,GL_STATIC_DRAW);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,eb); glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(qi),qi,GL_STATIC_DRAW);
      glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,4*sizeof(float),(void*)0); glEnableVertexAttribArray(0);
      glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,4*sizeof(float),(void*)8); glEnableVertexAttribArray(1);
    }

    // ── Shadow FBO ────────────────────────────────────────────────────────
    shadowDepth=makeTex(GL_DEPTH_COMPONENT,GL_DEPTH_COMPONENT,GL_FLOAT,
                        SHADOW_SZ,SHADOW_SZ,GL_CLAMP_TO_BORDER,GL_NEAREST);
    float bc[]={1,1,1,1};
    glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,bc);
    glGenFramebuffers(1,&shadowFBO);
    glBindFramebuffer(GL_FRAMEBUFFER,shadowFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,shadowDepth,0);
    glDrawBuffer(GL_NONE); glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER,0);

    // ── Transforms ────────────────────────────────────────────────────────
    rotation=QQuaternion();
    view.setToIdentity();
    view.lookAt({0.0f,1.5f,4.5f},{0.0f,-0.2f,0.0f},{0.0f,1.0f,0.0f});

    // Ortho shadow dari atas-depan, cukup lebar untuk cover semua objek
    lightProj.setToIdentity();
    lightProj.ortho(-5.0f,5.0f,-5.0f,5.0f,0.5f,25.0f);
    lightView.setToIdentity();
    lightView.lookAt({1.0f,8.0f,3.0f},{0,0,0},{0,1,0});

    prevRotMVP.setToIdentity();
    glEnable(GL_DEPTH_TEST);

    auto* t=new QTimer(this);
    connect(t,&QTimer::timeout,this,QOverload<>::of(&GLwidget::update));
    t->start(16);
  }

  // ══════════════════════════════════════════════════════════════════════════
  //  resizeGL
  // ══════════════════════════════════════════════════════════════════════════
  void resizeGL(int w, int h) override {
    W=w; H=h;
    glViewport(0,0,w,h);
    proj.setToIdentity();
    proj.perspective(55.0f,float(w)/float(h),0.1f,2000.0f);
    setupHDRFBO(w,h);
  }

  // ══════════════════════════════════════════════════════════════════════════
  //  paintGL  – 6 passes
  // ══════════════════════════════════════════════════════════════════════════
  void paintGL() override {
    // ── Animate main cube ─────────────────────────────────────────────────
    rotation = QQuaternion::fromAxisAndAngle(QVector3D(1,1,0),0.5f) * rotation;

    const QMatrix4x4 lightSpace = lightProj * lightView;
    const GLuint     defFBO     = defaultFramebufferObject();

    // ── Build per-object model matrices ───────────────────────────────────
    // 1. Kubus utama – berputar di tengah
    QMatrix4x4 mRotCube;
    mRotCube.setToIdentity();
    mRotCube.rotate(rotation);

    // 2. Kubus kecil – static kiri belakang
    QMatrix4x4 mCubeL;
    mCubeL.setToIdentity();
    mCubeL.translate(-2.2f, -0.55f, -1.8f);
    mCubeL.scale(0.55f);

    // 3. Kubus tinggi – static kanan
    QMatrix4x4 mCubeR;
    mCubeR.setToIdentity();
    mCubeR.translate(2.2f, -0.25f, -2.0f);
    mCubeR.scale(QVector3D(0.6f, 1.4f, 0.6f));

    // 4. Piramida – static kanan depan
    QMatrix4x4 mPyr;
    mPyr.setToIdentity();
    mPyr.translate(1.6f, -1.1f, 0.5f);
    mPyr.scale(0.9f, 1.2f, 0.9f);

    // 5. Sphere – static kiri
    QMatrix4x4 mSph;
    mSph.setToIdentity();
    mSph.translate(-2.0f, -0.45f, -0.8f);
    mSph.scale(0.55f);

    // 6. Floor – static, lebar
    QMatrix4x4 mFloor;
    mFloor.setToIdentity();
    mFloor.translate(0.0f, -1.1f, -1.0f);
    mFloor.scale(12.0f, 1.0f, 10.0f);

    // prevMVP untuk kubus utama (motion blur); objek static: prevMVP = currMVP
    const QMatrix4x4 currRotMVP = proj * view * mRotCube;
    // Untuk objek static, velocity = 0 → prevMVP = proj*view*staticModel
    auto staticPrev = [&](const QMatrix4x4& m){ return proj*view*m; };

    // ── PASS 1 : Shadow Map ───────────────────────────────────────────────
    glBindFramebuffer(GL_FRAMEBUFFER,shadowFBO);
    glViewport(0,0,SHADOW_SZ,SHADOW_SZ);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE); glCullFace(GL_FRONT);
    shadowSP.bind();
    shadowObject(cubeVAO,   36,          lightSpace * mRotCube);
    shadowObject(cubeVAO,   36,          lightSpace * mCubeL);
    shadowObject(cubeVAO,   36,          lightSpace * mCubeR);
    shadowObject(pyramidVAO,pyramidIdxCount, lightSpace * mPyr);
    shadowObject(sphereVAO, sphereIdxCount,  lightSpace * mSph);
    shadowObject(planeVAO,  6,           lightSpace * mFloor);
    glDisable(GL_CULL_FACE);

    // ── PASS 2 : Scene → HDR FBO ──────────────────────────────────────────
    glBindFramebuffer(GL_FRAMEBUFFER,hdrFBO);
    glViewport(0,0,W,H);
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    mainSP.bind();
    mainSP.setUniformValue("view",    view);
    mainSP.setUniformValue("proj",    proj);
    mainSP.setUniformValue("ViewPos", QVector3D(0.0f,1.5f,4.5f));
    mainSP.setUniformValue("shadowMap",0);
    bindTex(0,shadowDepth);

    // Setiap drawObject set: model, prevMVP, lightSpace, ObjectCol
    drawObject(cubeVAO,   36,         mRotCube, prevRotMVP,    lightSpace, {1.0f,0.50f,0.31f}); // orange
    drawObject(cubeVAO,   36,         mCubeL,   staticPrev(mCubeL), lightSpace, {0.20f,0.70f,0.80f}); // teal
    drawObject(cubeVAO,   36,         mCubeR,   staticPrev(mCubeR), lightSpace, {0.80f,0.20f,0.22f}); // merah
    drawObject(pyramidVAO,pyramidIdxCount, mPyr,  staticPrev(mPyr),  lightSpace, {0.25f,0.78f,0.35f}); // hijau
    drawObject(sphereVAO, sphereIdxCount,  mSph,  staticPrev(mSph),  lightSpace, {0.65f,0.25f,0.90f}); // ungu
    drawObject(planeVAO,  6,          mFloor,   staticPrev(mFloor),lightSpace, {0.70f,0.70f,0.70f}); // abu

    glBindFramebuffer(GL_FRAMEBUFFER,defFBO);

    // ── PASS 3 : Bloom ────────────────────────────────────────────────────
    glDisable(GL_DEPTH_TEST);
    blurSP.bind(); blurSP.setUniformValue("image",0);
    bool bh=true;
    for(int i=0;i<10;i++){
      glBindFramebuffer(GL_FRAMEBUFFER,bloomFBO[bh?0:1]);
      blurSP.setUniformValue("horizontal",bh);
      bindTex(0,(i==0)?hdrBright:bloomTex[bh?1:0]);
      drawMesh(quadVAO,6); bh=!bh;
    }
    glBindFramebuffer(GL_FRAMEBUFFER,defFBO);

    // ── PASS 4 : Motion Blur ──────────────────────────────────────────────
    glBindFramebuffer(GL_FRAMEBUFFER,motionFBO);
    glClear(GL_COLOR_BUFFER_BIT);
    motionSP.bind();
    motionSP.setUniformValue("scene",0);
    motionSP.setUniformValue("velocityMap",1);
    bindTex(0,hdrColor); bindTex(1,hdrVelocity);
    drawMesh(quadVAO,6);
    glBindFramebuffer(GL_FRAMEBUFFER,defFBO);

    // ── PASS 5 : Composite → default FBO ─────────────────────────────────
    glClearColor(0.96f,0.96f,0.96f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    compositeSP.bind();
    compositeSP.setUniformValue("scene",   0);
    compositeSP.setUniformValue("bloom",   1);
    compositeSP.setUniformValue("exposure",0.75f);
    bindTex(0,motionTex); bindTex(1,bloomTex[0]);
    drawMesh(quadVAO,6);

    // ── PASS 6 : Outer cube flat putih (depth blit) ───────────────────────
    glBindFramebuffer(GL_READ_FRAMEBUFFER,hdrFBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,defFBO);
    glBlitFramebuffer(0,0,W,H,0,0,W,H,GL_DEPTH_BUFFER_BIT,GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER,defFBO);
    glViewport(0,0,W,H);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_FALSE);
    flatSP.bind();
    flatSP.setUniformValue("view",view);
    flatSP.setUniformValue("proj",proj);
    drawMesh(skyVAO,36);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);

    prevRotMVP = currRotMVP;
  }
};
