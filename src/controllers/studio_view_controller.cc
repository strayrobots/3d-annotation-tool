#include <iostream>
#include <cassert>
#include "controllers/studio_view_controller.h"
#include "commands/keypoints.h"

using namespace commands;

StudioViewController::StudioViewController(SceneModel& model, CommandStack& stack) : sceneModel(model), commandStack(stack), camera(Vector3f(0.0, 0.0, 1.0), -2.0),
                                                                                     viewContext(camera), annotationController(model) {
}

void StudioViewController::viewWillAppear(int width, int height) {
  meshView = std::make_shared<views::MeshView>(width, height);
  meshDrawable = std::make_shared<views::MeshDrawable>(sceneModel.getMesh());
  meshView->addObject(meshDrawable);

  addKeypointTool = std::make_shared<AddKeypointTool>(sceneModel, *this, commandStack);
  moveKeypointTool = std::make_shared<MoveKeypointTool>(sceneModel, *this, annotationController, commandStack);
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
  if (currentTool->leftButtonDown(viewContext)) {
    return true;
  }
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
  viewContext.mousePositionX = x;
  viewContext.mousePositionY = y;
  if (!moved) {
    if (currentTool->leftButtonUp(viewContext)) {
      dragging = false;
      moved = false;
      return true;
    }
  } else {
    moved = false;
  }
  dragging = false;
  //if (annotationController.leftButtonUp(viewContext)) return true;
  return false;
}

bool StudioViewController::mouseMoved(double x, double y) {
  viewContext.mousePositionX = x;
  viewContext.mousePositionY = y;
  if (currentTool->mouseMoved(viewContext)) {
    return true;
  }
  if (annotationController.mouseMoved(viewContext)) {
    return true;
  }

  if (dragging) {
    moved = true;
    float diffX = (x - prevX);
    float diffY = (y - prevY);
    Quaternionf q = AngleAxisf(diffX * M_PI / 2000, Vector3f::UnitY()) * AngleAxisf(diffY * M_PI / 2000, Vector3f::UnitX());
    camera.rotateAroundTarget(q);

    prevX = x;
    prevY = y;
  }

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
