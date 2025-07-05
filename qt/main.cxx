#include <QApplication>

#include "Window.hxx"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  app.setApplicationName("QT test dev");
  app.setApplicationVersion("0.001 Alpha");
  app.setApplicationDisplayName("Window Test");

  Window window;
  window.show();

  return app.exec();
}
