#pragma once

#include <GL/gl.h>
#include <GL/glext.h>

#include <QOpenGLFunctions>
#include <iostream>
#include <obj3d.hxx>
#include <string>

#include "matrix.hxx"

namespace Engine {
using namespace Linear;
using namespace l3d;
class Util : protected QOpenGLFunctions {
  static Util* instance;

  Util() {
    initializeOpenGLFunctions();  // WAJIB setelah context aktif
  }

 public:
  static Util* get_instance() {
    if (!instance) instance = new Util();  // tidak const, karena akan pakai method-nya
    return instance;
  }

  std::string load_shader_file(const std::string& path) {
    std::string res;

    return res;
  }

  GLuint load_shader(GLenum type, const std::string& shaderCode) {
    GLuint      shader = glCreateShader(type);
    const char* src    = shaderCode.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    // Error checking
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
      char infoLog[512];
      glGetShaderInfoLog(shader, 512, nullptr, infoLog);
      std::cerr << "Shader compile error: " << infoLog << std::endl;
    }

    return shader;
  }

  GLuint create_program(const std::string& vsCode, const std::string& fsCode) {
    // Load dan compile vertex dan fragment shader
    GLuint vs = load_shader(GL_VERTEX_SHADER, vsCode);
    GLuint fs = load_shader(GL_FRAGMENT_SHADER, fsCode);

    // Buat program
    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    // Cek status link
    GLint linkStatus = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
    glValidateProgram(program);

    // Ambil log program
    char log[512];
    glGetProgramInfoLog(program, sizeof(log), nullptr, log);
    if (strlen(log) > 0) { std::cerr << "Link log: " << log << std::endl; }

    // Error handling
    if (linkStatus == GL_FALSE) {
      glDeleteProgram(program);
      std::cerr << "Error linking program." << std::endl;
      throw std::runtime_error("Error linking program:\n" + std::string(log));
    }
    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
  }

  void draw_object(const Mat4f& VPMat, const Obj3D<float>& obj) {
    const auto  vertices  = obj.get_processed_vertices();
    const Mat4f MVP       = VPMat * obj.get_model_matrix();
    const Mat4f normalMat = obj.get_model_matrix().inverse().transpose();
    // unimplemented yet glDrawArrays(GL_ARRAY_BUFFER, o=, GLsizei count)
  }
};

inline Util* Util::instance = nullptr;

}  // namespace Engine
