#include <iostream>
#include <cassert>
#include "controllers/studio_view_controller.h"
#include "commands/keypoints.h"

using namespace commands;

StudioViewController::StudioViewController(SceneModel& model, Timeline& tl) : sceneModel(model), timeline(tl), camera(),
  viewContext(camera), annotationView(model), sceneMeshView(model.getMesh()),
  addKeypointView(model, tl), moveKeypointView(model, tl), addBBoxView(model, tl) {
}

void StudioViewController::viewWillAppear(int width, int height) {
  auto meshMean = sceneModel.getMesh()->getMeshMean();
  camera.reset(meshMean, -meshMean.normalized());

  viewContext.width = width;
  viewContext.height = height;
}

views::View3D& StudioViewController::getActiveToolView() {
  if (sceneModel.activeToolId == AddKeypointToolId) {
    return addKeypointView;
  } else if (sceneModel.activeToolId == MoveKeypointToolId) {
    return moveKeypointView;
  } else {
    return addBBoxView;
  }
}

void StudioViewController::render() const {
  annotationView.render(viewContext);
  addBBoxView.render(viewContext);

  if (sceneModel.activeToolId == MoveKeypointToolId) {
    moveKeypointView.render(viewContext);
  }
  if (sceneModel.activeToolId == AddKeypointToolId) {
    sceneMeshView.render(viewContext);
  } else {
    sceneMeshView.render(viewContext, Matrix4f::Identity(), Vector4f(0.92, 0.59, 0.2, 0.35));
  }
}

// Input handling.
bool StudioViewController::leftButtonDown(double x, double y, InputModifier mod) {
  viewContext.mousePositionX = x;
  viewContext.mousePositionY = y;
  if (getActiveToolView().leftButtonDown(viewContext)) {
    return true;
  }
  dragging = true;
  moved = false;
  prevX = x;
  prevY = y;
  return true;
}

bool StudioViewController::leftButtonUp(double x, double y, InputModifier mod) {
  viewContext.mousePositionX = x;
  viewContext.mousePositionY = y;
  if (!moved) {
    if (getActiveToolView().leftButtonUp(viewContext)) {
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

bool StudioViewController::mouseMoved(double x, double y, InputModifier mod) {
  viewContext.mousePositionX = x;
  viewContext.mousePositionY = y;
  if (getActiveToolView().mouseMoved(viewContext)) {
    return true;
  }

  if (dragging) {
    moved = true;
    float diffX = float(x - prevX);
    float diffY = float(y - prevY);
    if (mod & ModCommand) {
      camera.translate(Vector3f(-diffX / float(viewContext.width), diffY / float(viewContext.height), 0));
    } else {
      Quaternionf q = AngleAxisf(diffX * M_PI / 2000, Vector3f::UnitY()) * AngleAxisf(diffY * M_PI / 2000, Vector3f::UnitX());
      camera.rotateAroundTarget(q);
    }
  }

  prevX = x;
  prevY = y;

  return true;
}

bool StudioViewController::scroll(double xoffset, double yoffset, InputModifier mod) {
  float diff = yoffset * 0.05;
  camera.zoom(diff);
  return true;
}

void StudioViewController::resize(int width, int height, InputModifier mod) {
  viewContext.width = width;
  viewContext.height = height;
}

bool StudioViewController::keypress(char character, InputModifier mod) {
  if (character == 'K') {
    sceneModel.activeToolId = AddKeypointToolId;
    return true;
  } else if (character == 'V') {
    sceneModel.activeToolId = MoveKeypointToolId;
    return true;
  } else if (character == 'B') {
    sceneModel.activeToolId = BBoxToolId;
    return true;
  }
  return false;
}

