#pragma once
#include "controllers/controller.h"
#include "views/mesh_view.h"

namespace controllers {
class AnnotationController : public Controller {
private:
public:
  std::shared_ptr<views::MeshView> meshView;


  AnnotationController() {
  }

  void viewWillAppear(int width, int height) override {
    meshView = std::make_shared<views::MeshView>(width, height);
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

