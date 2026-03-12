#include <ArrowPlotWidget.hpp>
#include <QApplication>
#include <QLabel>
#include <QMainWindow>
#include <QSurfaceFormat>

int main(int argc, char* argv[]) {
  QSurfaceFormat fmt;
  fmt.setVersion(3, 3);
  fmt.setProfile(QSurfaceFormat::CoreProfile);
  fmt.setSamples(4);
  QSurfaceFormat::setDefaultFormat(fmt);

  QApplication app(argc, argv);

  QMainWindow win;
  win.setWindowTitle("CollatzPath 3D — Qt6 OpenGL  |  drag=orbit  right-drag=pan  scroll=zoom  dbl-click=reset");
  win.resize(900, 700);

  auto* widget = new ArrowPlotWidget(&win);
  win.setCentralWidget(widget);

  // ── 3D data ───────────────────────────────────────────────────
  widget->plot().add({
      {0.f, 0.f, 0.f},
      {2.f, 3.f, 1.f},
      {5.f, 1.f, 3.f},
      {7.f, 4.f, 2.f},
      {9.f, 2.f, 5.f},
      {11.f, 5.f, 3.f},
  });

  // ── config ────────────────────────────────────────────────────
  app::Config cfg;
  cfg.nodeRadius   = 0.05f;
  cfg.sphereStacks = 16;
  cfg.sphereSlices = 24;
  cfg.shaftRadius  = 0.012f;
  cfg.shaftSlices  = 14;
  cfg.headLength   = 0.07f;
  cfg.headRadius   = 0.032f;
  cfg.headSlices   = 20;
  cfg.padding      = 0.15f;
  widget->config() = cfg;

  // ── colours ───────────────────────────────────────────────────
  ap::ColourScheme col;
  col.nodeR         = 1.f;
  col.nodeG         = 1.f;
  col.nodeB         = 1.f;
  col.nodeA         = 1.f;
  col.shaftR        = 0.f;
  col.shaftG        = 0.83f;
  col.shaftB        = 0.67f;
  col.shaftA        = 1.f;
  col.headR         = 1.f;
  col.headG         = 0.45f;
  col.headB         = 0.f;
  col.headA         = 1.f;
  widget->colours() = col;

  win.show();
  return app.exec();
}
