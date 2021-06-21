#include <iostream>
#include <cassert>
#include "controllers/studio_view_controller.h"
#include "commands/keypoints.h"

using namespace commands;

StudioViewController::StudioViewController(SceneModel& model, Timeline& tl, int viewId) : viewId(viewId), sceneModel(model),
                                                                                          viewContext(sceneModel.sceneCamera()), annotationView(model, viewId), sceneMeshView(model.getMesh(), viewId),
                                                                                          addKeypointView(model, tl, viewId), moveKeypointView(model, tl, viewId), addBBoxView(model, tl, viewId),
                                                                                          statusBarView(model) {}

void StudioViewController::viewWillAppear(int width, int height) {
  viewContext.camera.reset(Vector3f::UnitZ(), Vector3f::Zero());

  viewContext.width = width;
  viewContext.height = height - views::StatusBarHeight;

  bgfx::setViewClear(viewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);
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

void StudioViewController::refresh() {
  addBBoxView.refresh();
  moveKeypointView.refresh();
}

void StudioViewController::render() const {
  bgfx::setViewRect(viewId, 0, 0, viewContext.width, viewContext.height);
  annotationView.render(viewContext);

  if (sceneModel.activeToolId == MoveKeypointToolId) {
    moveKeypointView.render(viewContext);
  } else if (sceneModel.activeToolId == BBoxToolId) {
    addBBoxView.render(viewContext);
  }
  if (sceneModel.activeToolId == AddKeypointToolId) {
    sceneMeshView.render(viewContext);
  } else {
    sceneMeshView.render(viewContext, Matrix4f::Identity(), Vector4f(0.92, 0.59, 0.2, 0.35));
  }
  views::Rect rect = {.x = 0, .y = float(viewContext.height), .width = float(viewContext.width), .height = float(views::StatusBarHeight)};
  statusBarView.render(rect);
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
      viewContext.camera.translate(Vector3f(-diffX / float(viewContext.width), diffY / float(viewContext.height), 0));
    } else {
      Quaternionf q = AngleAxisf(diffX * M_PI / 2000, Vector3f::UnitY()) * AngleAxisf(diffY * M_PI / 2000, Vector3f::UnitX());
      viewContext.camera.rotateAroundTarget(q);
    }
  }

  prevX = x;
  prevY = y;

  return true;
}

bool StudioViewController::scroll(double xoffset, double yoffset, InputModifier mod) {
  float diff = yoffset * 0.05;
  viewContext.camera.zoom(diff);
  return true;
}

void StudioViewController::resize(int width, int height, InputModifier mod) {
  viewContext.width = width;
  viewContext.height = height - views::StatusBarHeight;
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
  } else if ('0' <= character && character <= '9') {
    const int codePoint0Char = 48;
    int integerValue = int(character) - codePoint0Char;
    sceneModel.currentInstanceId = integerValue;
    getActiveToolView().keypress(character, mod);
  }
  return false;
}
