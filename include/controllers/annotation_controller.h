#pragma once
#include <vector>
#include <algorithm>
#include "controllers/controller.h"
#include "views/mesh_view.h"
#include "views/controls/translate.h"

namespace controllers {
class AnnotationController : public Controller {
private:
  std::vector<std::shared_ptr<views::controls::Control>> controls;
public:
  std::shared_ptr<views::MeshView> meshView;

  AnnotationController() : controls() {
  }

  void viewWillAppear(int width, int height) override {
    meshView = std::make_shared<views::MeshView>(width, height);
  }

  void addControl(std::shared_ptr<views::controls::Control> control) {
    controls.push_back(control);
  }

  void removeControl(std::shared_ptr<views::controls::Control> control) {
    auto item = std::find(std::begin(controls), std::end(controls), control);
    if (item == std::end(controls)) {
      std::cout << "Can't find control. This should not happen. Please file a bug report." << std::endl;
    }
    controls.erase(item);
  }

  // Mouse events.
  bool leftButtonDown(const ViewContext3D& viewContext) override {
    bool hitControl = false;
    for (auto& control : controls) {
      hitControl = control->leftButtonDown(viewContext);
    }

    return hitControl;
  }

  bool leftButtonUp(const ViewContext3D& viewContext) override {
    bool hitControl = false;
    for (auto& control : controls) {
      hitControl = control->leftButtonUp(viewContext);
    }

    return hitControl;
  }

  bool mouseMoved(const ViewContext3D& viewContext) override {
    bool hitControl = false;
    for (auto& control : controls) {
      hitControl = control->mouseMoved(viewContext);
    }
    return hitControl;
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
    for (auto& control : controls) {
      control->render(camera);
    }
  }
};
}

