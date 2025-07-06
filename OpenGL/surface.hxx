#include <QOpenGLWidget>
#include <matrix.hxx>
#include <obj3d.hxx>

#include "util.hxx"

namespace Engine {
using namespace Linear;
using namespace l3d;
class Surface : public QOpenGLWidget {
  std::vector<Obj3D<float>> objects;
  Mat4f                     viewMat, projectionMat;

 public:
  Surface() : QOpenGLWidget() {
    using namespace Linear;

    // NOTE : this cube jsut some testing later
    // 8 titik kubus (ukuran 1, dari -0.5 ke 0.5)
    std::vector<Vec3<float>> cubeVertices = {
        {-0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, -0.5f}, {0.5f, 0.5f, -0.5f}, {-0.5f, 0.5f, -0.5f},
        {-0.5f, -0.5f, 0.5f},  {0.5f, -0.5f, 0.5f},  {0.5f, 0.5f, 0.5f},  {-0.5f, 0.5f, 0.5f},
    };

    // 12 triangle face (dari 6 sisi * 2 triangle per sisi)
    std::vector<Vec3<short>> cubeFaces = {
        // belakang
        {0, 1, 2},
        {0, 2, 3},
        // depan
        {4, 6, 5},
        {4, 7, 6},
        // kiri
        {0, 3, 7},
        {0, 7, 4},
        // kanan
        {1, 5, 6},
        {1, 6, 2},
        // bawah
        {0, 4, 5},
        {0, 5, 1},
        // atas
        {3, 2, 6},
        {3, 6, 7},
    };

    Obj3D<float> cube(cubeVertices, cubeFaces);
    objects.push_back(std::move(cube));
  }

  void set_view_matrix(const Mat4f& viewMat) { this->viewMat = viewMat; }
  void set_projection_matrix(const Mat4f& projectionMat) { this->projectionMat = projectionMat; }

  void initializeGL() override {
    // Setup GL state
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
  }

  void resizeGL(int w, int h) override {
    glViewport(0, 0, w, h);
    ;
  }

  void paintGL() override {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Util* util  = Util::get_instance();
    Mat4f VPMat = projectionMat * viewMat;
    for (size_t i = 0; i < objects.size(); ++i) util->draw_object(VPMat, objects[i]);
  }
};
}  // namespace Engine
