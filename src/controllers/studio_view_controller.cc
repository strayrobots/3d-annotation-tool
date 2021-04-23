#include <iostream>
#include <cassert>
#include "controllers/studio_view_controller.h"
#include "commands/keypoints.h"

using namespace commands;

StudioViewController::StudioViewController(SceneModel& model) : sceneModel(model), camera(Vector3f(0.0, 0.0, 1.0), -2.0), viewContext(camera), annotationController() {
}

void StudioViewController::viewWillAppear(int width, int height) {
  meshView = std::make_shared<views::MeshView>(width, height);
  meshDrawable = std::make_shared<views::MeshDrawable>(sceneModel.getMesh());
  meshView->addObject(meshDrawable);

  addKeypointTool = std::make_shared<AddKeypointTool>(sceneModel);
  moveKeypointTool = std::make_shared<MoveKeypointTool>(sceneModel, meshView, annotationController);
  currentTool = addKeypointTool;

  viewContext.width = width;
  viewContext.height = height;
  annotationController.viewWillAppear(width, height);
}

void StudioViewController::render() const {
  assert(meshView != nullptr && "Rendering not initialized");

  annotationController.render(camera);
  meshView->render(camera);
}

// Input handling.
bool StudioViewController::leftButtonDown(double x, double y) {
  viewContext.mousePositionX = x;
  viewContext.mousePositionY = y;
  if (annotationController.leftButtonDown(viewContext)) {
    return true;
  }
  dragging = true;
  moved = false;
  prevX = x;
  prevY = y;
  return true;
}

bool StudioViewController::leftButtonUp(double x, double y) {
  if (annotationController.leftButtonUp(viewContext)) return true;
  dragging = false;
  if (!moved) {
    auto optionalCommand = currentTool->leftClick(pointingAt);
    if (optionalCommand.has_value()) {
      optionalCommand.value()->execute(*this, sceneModel);
      commandStack.push_back(std::move(optionalCommand.value()));
      return true;
    }
  }
  return false;
}

bool StudioViewController::mouseMoved(double x, double y) {
  viewContext.mousePositionX = x;
  viewContext.mousePositionY = y;
  if (annotationController.mouseMoved(viewContext)) {
    return true;
  }

  moved = true;
  if (dragging) {
    float diffX = (x - prevX);
    float diffY = (y - prevY);
    Quaternionf q = AngleAxisf(diffX*M_PI/2000, Vector3f::UnitY())
                          * AngleAxisf(diffY*M_PI/2000, Vector3f::UnitX());
    camera.rotateAroundTarget(q);

    prevX = x;
    prevY = y;
  }

  const Vector3f& rayDirection = camera.computeRayWorld(viewContext.width, viewContext.height, x, y);
  pointingAt = sceneModel.traceRay(camera.getPosition(), rayDirection);
  return true;
}

bool StudioViewController::scroll(double xoffset, double yoffset) {
  float diff = yoffset * 0.05;
  camera.zoom(diff);
  return true;
}

void StudioViewController::resize(int width, int height) {
  viewContext.width = width;
  viewContext.height = height;
  meshView->resize(width, height);
}

bool StudioViewController::keypress(char character) {
  if (character == 'K') {
    currentTool->deactivate();
    currentTool = addKeypointTool;
    currentTool->activate();
    return true;
  } else if (character == 'V') {
    currentTool->deactivate();
    currentTool = moveKeypointTool;
    currentTool->activate();
    return true;
  }
  return false;
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


