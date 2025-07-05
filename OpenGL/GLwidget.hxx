#pragma once
#include <QOpenGLWidget>

class GLwidget : public QOpenGLWidget {
 protected:
  void initializeGL() override {
    // Setup OpenGL state here
  }

  void resizeGL(int w, int h) override { glViewport(0, 0, w, h); }

  void paintGL() override {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
  }
};
