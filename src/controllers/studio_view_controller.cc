#include <iostream>
#include <cassert>
#include "controllers/studio_view_controller.h"
#include "commands/keypoints.h"
#include "id.h"

using namespace commands;
using namespace views;

StudioViewController::StudioViewController(SceneModel& model, Timeline& tl) : viewId(IdFactory::getInstance().getId()), sceneModel(model),
                                                                              viewContext(sceneModel.sceneCamera()),
                                                                              annotationView(model, viewId),
                                                                              sceneMeshView(model.getMesh(), viewId),
                                                                              pointCloudView(model, viewId),
                                                                              addKeypointView(model, tl, viewId),
                                                                              moveToolView(model, tl, viewId),
                                                                              addBBoxView(model, tl, viewId),
                                                                              addRectangleView(model, tl, viewId),
                                                                              statusBarView(model, IdFactory::getInstance().getId()) {
  imageSize = model.imageSize();
  preview = std::make_shared<controllers::PreviewController>(model, IdFactory::getInstance().getId());
  addSubController(std::static_pointer_cast<controllers::Controller>(preview));
}

void StudioViewController::viewWillAppear(const views::Rect& rect) {
  viewContext.camera.reset(Vector3f::UnitZ(), Vector3f::Zero());

  viewContext.width = rect.width;
  viewContext.height = rect.height - views::StatusBarHeight;

  bgfx::setViewClear(viewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);
  preview->viewWillAppear(previewRect());
  statusBarView.setRect(statusBarRect());
}

views::View3D& StudioViewController::getActiveToolView() {
  if (sceneModel.activeToolId == AddKeypointToolId) {
    return addKeypointView;
  } else if (sceneModel.activeToolId == MoveKeypointToolId) {
    return moveToolView;
  } else if (sceneModel.activeToolId == BBoxToolId) {
    return addBBoxView;
  } else {
    return addRectangleView;
  }
}

void StudioViewController::refresh() {
  addBBoxView.refresh();
  moveToolView.refresh();
}

void StudioViewController::render() const {
  bgfx::setViewRect(viewId, 0, 0, viewContext.width, viewContext.height);
  annotationView.render(viewContext);

  if (sceneModel.activeToolId == MoveKeypointToolId) {
    moveToolView.render(viewContext);
  } else if (sceneModel.activeToolId == BBoxToolId) {
    addBBoxView.render(viewContext);
  } else if (sceneModel.activeToolId == AddRectangleToolId) {
    addRectangleView.render(viewContext);
  }

  if (sceneModel.activeView == active_view::MeshView) {
    if (sceneModel.activeToolId == AddKeypointToolId) {
      sceneMeshView.render(viewContext);
    } else {
      sceneMeshView.render(viewContext, Matrix4f::Identity(), Vector4f(0.92, 0.59, 0.2, 0.5));
    }
  } else {
    pointCloudView.render(viewContext);
  }
  statusBarView.render();
  preview->render();
}

// Input handling.
bool StudioViewController::leftButtonDown(double x, double y, InputModifier mod) {
  updateViewContext(x, y, mod);

  if (preview->leftButtonDown(viewContext)) {
    return true;
  }

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
  updateViewContext(x, y, mod);

  if (!moved) {
    if (preview->leftButtonUp(viewContext)) {
      return true;
    }
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
  updateViewContext(x, y, mod);

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

void StudioViewController::resize(const views::Rect& rect) {
  viewContext.width = rect.width;
  viewContext.height = rect.height - views::StatusBarHeight;
  preview->resize(previewRect());
  statusBarView.setRect(statusBarRect());
}

bool StudioViewController::keypress(char character, const InputModifier mod) {
  Controller::keypress(character, mod);
  if (sceneModel.activeView == active_view::PointCloudView) {
    if (mod & ModCtrl && (character == '+' || character == '=')) {
      pointCloudView.changeSize(1.0f);
      return true;
    } else if (mod & ModCtrl && character == '-') {
      pointCloudView.changeSize(-1.0f);
      return true;
    }
  }
  if (mod == ModShift && character == '1') {
    sceneModel.activeView = active_view::MeshView;
  } else if (mod == ModShift && character == '2') {
    pointCloudView.loadPointCloud();
    sceneModel.activeView = active_view::PointCloudView;
  } else if (character == 'K') {
    sceneModel.activeToolId = AddKeypointToolId;
    return true;
  } else if (character == 'V' && mod == ModNone) {
    sceneModel.activeToolId = MoveKeypointToolId;
    return true;
  } else if (character == 'B') {
    sceneModel.activeToolId = BBoxToolId;
    return true;
  } else if (character == 'R' && mod == ModNone) {
    sceneModel.activeToolId = AddRectangleToolId;
    return true;
  } else if ('0' <= character && character <= '9') {
    const int codePoint0Char = 48;
    int integerValue = int(character) - codePoint0Char;
    if (integerValue < sceneModel.datasetMetadata.numClasses) {
      sceneModel.currentInstanceId = integerValue;
      getActiveToolView().keypress(character, mod);
    }
  }
  return false;
}

views::Rect StudioViewController::previewRect() const {
  float aspectRatio = float(imageSize.second) / float(imageSize.first);
  float previewWidth = 0.25 * viewContext.width;
  float previewHeight = aspectRatio * previewWidth;
  return {
      .x = float(viewContext.width - previewWidth),
      .y = 0.0,
      .width = previewWidth,
      .height = previewHeight};
}

views::Rect StudioViewController::statusBarRect() const {
  return {.x = 0, .y = float(viewContext.height), .width = float(viewContext.width), .height = float(views::StatusBarHeight)};
}

void StudioViewController::updateViewContext(double x, double y, InputModifier mod) {
  viewContext.modifiers = mod;
  viewContext.mousePositionX = x;
  viewContext.mousePositionY = y;

  const Vector3f& rayDirection = viewContext.camera.computeRayWorld(viewContext.width, viewContext.height,
                                                                    viewContext.mousePositionX, viewContext.mousePositionY);
  auto intersection = sceneModel.traceRayIntersection(viewContext.camera.getPosition(), rayDirection);
  if (intersection.hit) {
    viewContext.pointingAt = intersection.point;
    viewContext.pointingAtNormal = intersection.normal;
  } else {
    viewContext.pointingAt = {};
    viewContext.pointingAtNormal = {};
  }
}
