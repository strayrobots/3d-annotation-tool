#include <iostream>
#include "controllers/studio_view_controller.h"
#include "commands/keypoints.h"

using namespace commands;

StudioViewController::StudioViewController(SceneModel& model) : sceneModel(model), camera(Vector3f(0.0, 1.0, 0.0)), viewContext(camera) {
  addKeypointTool = std::make_shared<AddKeypointTool>(model);
  currentTool = addKeypointTool;

  meshView = std::make_shared<views::MeshView>();
  meshDrawable = std::make_shared<views::MeshDrawable>(sceneModel.getMesh());
  meshView->addObject(meshDrawable);
}

void StudioViewController::render() const {
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
  if (!moved) {
    auto optionalCommand = currentTool->leftClick(pointingAt);
    if (optionalCommand.has_value()) {
      optionalCommand.value()->execute(*this, sceneModel);
      commandStack.push_back(std::move(optionalCommand.value()));
    }
  }
}

void StudioViewController::mouseMoved(double x, double y) {
  moved = true;
  viewContext.mousePositionX = x;
  viewContext.mousePositionY = y;
  if (dragging) {
    float diffX = (x - prevX) * M_PI / 1000.0;
    float diffY = (y - prevY) * M_PI / 1000.0;
    Quaternionf rotationX, rotationY;
    rotationY = AngleAxis(diffY, camera.getOrientation() * Vector3f::UnitX());
    rotationX = AngleAxis(diffX, camera.getOrientation() * Vector3f::UnitY());
    camera.setOrientation(rotationX * rotationY * camera.getOrientation());

    prevX = x;
    prevY = y;
  }

  const Vector3f& rayDirection = camera.computeRay(viewContext.width, viewContext.height, x, y);
  pointingAt = sceneModel.traceRay(camera.getPosition(), rayDirection);
}

void StudioViewController::scroll(double xoffset, double yoffset) {
  double diff = yoffset * 0.05;
  const auto& cameraPosition = camera.getPosition();
  double newNorm = std::max(cameraPosition.norm() + diff, 0.1);
  camera.setPosition(newNorm * cameraPosition.normalized());
}

void StudioViewController::keypress(char character) {
  if (character == 'K') {
    currentTool = addKeypointTool;
  }
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


