/*
  cpp-playground - C++ experiments and learning playground
  Copyright (C) 2025 M. Reza Dwi Prasetiawan


  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <https://www.gnu.org/licenses/>.
*/


#pragma once
#include <QMatrix4x4>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QQuaternion>
#include <QTimer>
#include <QVector>

// this is just template from my past android gles java playground converted to qt cpp playgrond, i need to remember how exactly the calculation work
class GLwidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {
  Q_OBJECT

 public:
  explicit GLwidget(QWidget* parent = nullptr) : QOpenGLWidget(parent) {}

 protected:
  GLuint               vao = 0, vbo = 0;
  QOpenGLShaderProgram shader;
  QMatrix4x4           model, view, proj;
  QQuaternion          rotation;

  void initializeGL() override {
    initializeOpenGLFunctions();

    rotation      = QQuaternion();
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, QOverload<>::of(&GLwidget::update));
    timer->start(16);

    const char* vertexShaderSrc = R"(#version 330 core
    layout(location = 0) in vec3 aPos;
    layout(location = 1) in vec3 aNormal;

    out vec3 FragPos;
    out vec3 Normal;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    void main() {
      FragPos = vec3(model * vec4(aPos, 1.0));
      Normal = mat3(transpose(inverse(model))) * aNormal;
      gl_Position = projection * view * vec4(FragPos, 1.0);
    })";

    const char* fragmentShaderSrc = R"(#version 330 core
    in vec3 FragPos;
    in vec3 Normal;

    uniform vec3 LightPos;
    uniform vec3 ViewPos;
    uniform vec3 LightCol;
    uniform vec3 ObjectCol;

    out vec4 FragColor;

    void main()
    {
        float ambientStrength = 0.1;
        vec3 ambient = ambientStrength * LightCol;

        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(LightPos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * LightCol;

        float specularStrength = 1;
        vec3 viewDir = normalize(ViewPos - FragPos);
        vec3 halfWayDir = normalize(viewDir + lightDir);
        float spec = pow(max(dot(norm, halfWayDir), 0.0), 32.0);
        vec3 specular = specularStrength * spec * LightCol;

        vec3 result = (ambient + diffuse) * ObjectCol + specular;
        FragColor = vec4(result, 1.0);
    })";

    shader.addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSrc);
    shader.addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSrc);
    shader.link();

    // Simple cube with normals
    float vertices[] = {
        // pos             // normal
        -0.5, -0.5, -0.5, 0,  0,  -1, 0.5,  -0.5, -0.5, 0,  0,  -1, 0.5,  0.5,  -0.5, 0,  0,  -1, -0.5, 0.5,  -0.5, 0,  0,  -1,
        -0.5, -0.5, 0.5,  0,  0,  1,  0.5,  -0.5, 0.5,  0,  0,  1,  0.5,  0.5,  0.5,  0,  0,  1,  -0.5, 0.5,  0.5,  0,  0,  1,

        -0.5, 0.5,  0.5,  -1, 0,  0,  -0.5, 0.5,  -0.5, -1, 0,  0,  -0.5, -0.5, -0.5, -1, 0,  0,  -0.5, -0.5, 0.5,  -1, 0,  0,

        0.5,  0.5,  0.5,  1,  0,  0,  0.5,  0.5,  -0.5, 1,  0,  0,  0.5,  -0.5, -0.5, 1,  0,  0,  0.5,  -0.5, 0.5,  1,  0,  0,

        -0.5, -0.5, -0.5, 0,  -1, 0,  0.5,  -0.5, -0.5, 0,  -1, 0,  0.5,  -0.5, 0.5,  0,  -1, 0,  -0.5, -0.5, 0.5,  0,  -1, 0,

        -0.5, 0.5,  -0.5, 0,  1,  0,  0.5,  0.5,  -0.5, 0,  1,  0,  0.5,  0.5,  0.5,  0,  1,  0,  -0.5, 0.5,  0.5,  0,  1,  0,
    };

    unsigned int indices[] = {
        0,  1,  2,  2,  3,  0,   // back
        4,  5,  6,  6,  7,  4,   // front
        8,  9,  10, 10, 11, 8,   // left
        12, 13, 14, 14, 15, 12,  // right
        16, 17, 18, 18, 19, 16,  // bottom
        20, 21, 22, 22, 23, 20   // top
    };

    GLuint vbo, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glEnable(GL_DEPTH_TEST);

    model.setToIdentity();
    view.lookAt(QVector3D(0, 0, 3), QVector3D(0, 0, 0), QVector3D(0, 1, 0));
  }

  void resizeGL(int w, int h) override {
    glViewport(0, 0, w, h);
    proj.setToIdentity();
    proj.frustum(-(float)w / h, (float)w / h, -1, 1, 1.0f, 5.0f);
  }

  void paintGL() override {
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    rotation = QQuaternion::fromAxisAndAngle(QVector3D(1, 1, 0), 0.5f) * rotation;
    model.setToIdentity();
    model.rotate(rotation);

    shader.bind();
    shader.setUniformValue("model", model);
    shader.setUniformValue("view", view);
    shader.setUniformValue("projection", proj);

    shader.setUniformValue("LightPos", QVector3D(0, 0, 3.0f));
    shader.setUniformValue("ViewPos", QVector3D(1.5f, 1.5f, 2.0f));
    shader.setUniformValue("LightCol", QVector3D(0.8f, 0.8f, 0.8f));
    shader.setUniformValue("ObjectCol", QVector3D(1, 0.5f, 0.31f));

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
  }
};
