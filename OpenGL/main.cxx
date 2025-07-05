#include <QApplication>
#include <QScreen>

#include "GLwidget.hxx"

int main(int argc, char** argv) {
  QApplication app(argc, argv);

  // Optional: request OpenGL 3.3 core
  QSurfaceFormat fmt;
  fmt.setVersion(3, 3);
  fmt.setProfile(QSurfaceFormat::CoreProfile);
  QSurfaceFormat::setDefaultFormat(fmt);

  // Ambil ukuran layar utama
  QScreen* screen = QGuiApplication::primaryScreen();
  int screenWidth = screen->geometry().width();
  int screenHeight = screen->geometry().height();

  // Buat window dengan ukuran full screen (atau bisa ¾ layar)
  GLwidget w;
  w.resize(screenWidth, screenHeight);
  w.show();
  return app.exec();
}