#pragma once
#include "controllers/controller.h"
#include "views/mesh_view.h"
#include "geometry/mesh.h"

namespace controllers {
class AnnotationController : public Controller {
private:
  std::shared_ptr<views::MeshDrawable> xAxisControl;
  std::shared_ptr<views::MeshDrawable> yAxisControl;
  std::shared_ptr<views::MeshDrawable> zAxisControl;
public:
  std::shared_ptr<views::MeshView> meshView;

  AnnotationController() {
    auto xTransform = Transform<float, 3, Eigen::Affine>::Identity();
    auto yTransform = Transform<float, 3, Eigen::Affine>::Identity();
    auto zTransform = Transform<float, 3, Eigen::Affine>::Identity();
    yTransform.rotate(AngleAxisf(-M_PI/2.0, Vector3f(0.0, 0.0, 1.0)));
    zTransform.rotate(AngleAxisf(M_PI/2.0, Vector3f(0.0, 1.0, 0.0)));

    auto meshX = std::make_shared<geometry::Mesh>("../assets/x_axis.ply", xTransform.matrix(), 0.5);
    auto meshY = std::make_shared<geometry::Mesh>("../assets/x_axis.ply", yTransform.matrix(), 0.5);
    auto meshZ = std::make_shared<geometry::Mesh>("../assets/x_axis.ply", zTransform.matrix(), 0.5);
    xAxisControl = std::make_shared<views::MeshDrawable>(meshX, Vector4f(1.0, 0.2, 0.2, 1.0));
    yAxisControl = std::make_shared<views::MeshDrawable>(meshY, Vector4f(0.2, 1.0, 0.2, 1.0));
    zAxisControl = std::make_shared<views::MeshDrawable>(meshZ, Vector4f(0.2, 0.2, 1.0, 1.0));
  }

  void viewWillAppear(int width, int height) override {
    meshView = std::make_shared<views::MeshView>(width, height);
    meshView->addObject(xAxisControl);
    meshView->addObject(yAxisControl);
    meshView->addObject(zAxisControl);
  }

  void addKeypoint(const Vector3f& position) {
    Matrix4f T = Matrix4f::Identity();
    T.block(0, 3, 3, 1) = position;
    auto sphere = std::make_shared<geometry::Sphere>(T, 0.01);
    auto sphereDrawable = std::make_shared<views::MeshDrawable>(sphere, KeypointColor);
    meshView->addObject(sphereDrawable);
  }

  void removeKeypoint(const Vector3f& position) {
    meshView->popObject();
  }

  void render(const Camera& camera) const {
    meshView->render(camera);
  }
};
}

