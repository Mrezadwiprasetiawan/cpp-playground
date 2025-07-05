#include <QOpenGLWidget>
#include <obj3d.hxx>

namespace Engine {
class Surface : public QOpenGLWidget {
  std::vector<l3d::Obj3D<float>> objects;

 public:
  void initializeGL() override {}
  void resizeGL(int w, int h) override {}
  void paintGL() override {}
};
}  // namespace Engine