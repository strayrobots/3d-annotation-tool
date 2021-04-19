#include <iostream>
#include <cassert>
#include "controllers/studio_view_controller.h"
#include "commands/keypoints.h"

using namespace commands;

StudioViewController::StudioViewController(SceneModel& model) : sceneModel(model), camera(Vector3f(0.0, 0.0, 1.0), -2.0), viewContext(camera) {
  addKeypointTool = std::make_shared<AddKeypointTool>(model);
  currentTool = addKeypointTool;
  annotationController = std::make_shared<controllers::AnnotationController>();
}

void StudioViewController::viewWillAppear(int width, int height) {
  viewContext.width = width;
  viewContext.height = height;
  annotationController->viewWillAppear(width, height);
  meshView = std::make_shared<views::MeshView>(width, height);
  meshDrawable = std::make_shared<views::MeshDrawable>(sceneModel.getMesh());
  meshView->addObject(meshDrawable);
}

void StudioViewController::render() const {
  assert(meshView != nullptr && "Rendering not initialized");

  meshView->render(camera);
  annotationController->render(camera);
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
}

void StudioViewController::scroll(double xoffset, double yoffset) {
  float diff = yoffset * 0.05;
  camera.zoom(diff);
}

void StudioViewController::resize(int width, int height) {
  viewContext.width = width;
  viewContext.height = height;
  meshView->resize(width, height);
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


