#include <iostream>
#include "controllers/studio_view_controller.h"
#include "commands/keypoints.h"

using namespace commands;

StudioViewController::StudioViewController(SceneModel& model) : sceneModel(model) {
  meshView = std::make_shared<views::MeshView>();
  meshDrawable = std::make_shared<views::MeshDrawable>(sceneModel.getMesh());
  meshView->addObject(meshDrawable);
}

void StudioViewController::render(const Camera& camera) const {
  meshView->render(camera);
}

// Input handling.
void StudioViewController::leftButtonDown(double x, double y) {
  dragging = true;
  moved = false;
  prevX = x;
  prevY = y;
}

void StudioViewController::leftButtonUp(double x, double y) {
  dragging = false;
  if (!moved && pointingAt.has_value()) {
    std::unique_ptr<Command> command = std::make_unique<AddKeypointCommand>(pointingAt.value());
    command->execute(*this, sceneModel);
    commandStack.push_back(std::move(command));
    std::cout << "Added keypoint: " << pointingAt.value().transpose() << std::endl;
  }
}

void StudioViewController::mouseMoved(double x, double y) {
  moved = true;
  if (dragging) {
    float diffX = (x - prevX) * M_PI / 1000.0;
    float diffY = (y - prevY) * M_PI / 1000.0;
    Quaternionf rotationX, rotationY;
    const auto& camera = sceneModel.getCamera();
    rotationY = AngleAxis(diffY, camera.getOrientation() * Vector3f::UnitX());
    rotationX = AngleAxis(diffX, camera.getOrientation() * Vector3f::UnitY());
    sceneModel.setCameraOrientation(rotationX * rotationY * camera.getOrientation());

    prevX = x;
    prevY = y;
  }
  pointingAt = sceneModel.traceRay(x, y);
}

// Commands.
void StudioViewController::undo() {
  if (commandStack.empty()) return;
  commandStack.back()->undo(*this, sceneModel);
  commandStack.pop_back();
}

void StudioViewController::pushCommand(std::unique_ptr<Command> command) {
  commandStack.push_back(std::move(command));
}


