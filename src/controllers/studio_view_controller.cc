#include <iostream>
#include <cassert>
#include "controllers/studio_view_controller.h"
#include "commands/keypoints.h"

using namespace commands;

StudioViewController::StudioViewController(SceneModel& model, Timeline& tl) : sceneModel(model), timeline(tl), camera(Vector3f(0.0, 0.0, 1.0), -2.0),
                                                                                     viewContext(camera),
  annotationView(model), sceneMeshView(model.getMesh()) {
}

void StudioViewController::viewWillAppear(int width, int height) {
  viewContext.width = width;
  viewContext.height = height;

  addKeypointTool = std::make_shared<AddKeypointTool>(sceneModel, timeline);
  moveKeypointTool = std::make_shared<MoveKeypointTool>(sceneModel, timeline);
}

std::shared_ptr<tools::Tool> StudioViewController::getActiveTool() const {
  if (sceneModel.activeToolId == AddKeypointToolId) {
    return addKeypointTool;
  } else {
    return moveKeypointTool;
  }
}

void StudioViewController::render() const {
  annotationView.render(viewContext);
  if (sceneModel.activeToolId == AddKeypointToolId) {
    sceneMeshView.render();
  } else {
    sceneMeshView.render(Matrix4f::Identity(), Vector4f(0.92, 0.59, 0.2, 0.35));
  }
}

// Input handling.
bool StudioViewController::leftButtonDown(double x, double y) {
  viewContext.mousePositionX = x;
  viewContext.mousePositionY = y;
  if (getActiveTool()->leftButtonDown(viewContext)) {
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
    if (getActiveTool()->leftButtonUp(viewContext)) {
      dragging = false;
      moved = false;
      return true;
    }
  } else {
    moved = false;
  }
  dragging = false;
  return false;
}

bool StudioViewController::mouseMoved(double x, double y) {
  viewContext.mousePositionX = x;
  viewContext.mousePositionY = y;
  if (getActiveTool()->mouseMoved(viewContext)) {
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
}

bool StudioViewController::keypress(char character) {
  if (character == 'K') {
    sceneModel.activeToolId = AddKeypointToolId;
    return true;
  } else if (character == 'V') {
    sceneModel.activeToolId = MoveKeypointToolId;
    return true;
  }
  return false;
}

