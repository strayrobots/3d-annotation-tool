#include <iostream>
#include <cassert>
#include "controllers/studio_view_controller.h"
#include "commands/keypoints.h"
#include "commands/bounding_box.h"
#include "id.h"
#include "3rdparty/json.hpp"
#include "utils/serialize.h"
#include "utils/dataset.h"

namespace fs = std::filesystem;
using json = nlohmann::json;

using namespace commands;
using namespace views;

StudioViewController::StudioViewController(fs::path datasetPath) : viewId(IdFactory::getInstance().getId()),
                                                                   sceneModel((datasetPath / "scene" / "integrated.ply").string()),
                                                                   sceneCamera(datasetPath / "camera_intrinsics.json"),
                                                                   datasetPath(datasetPath),
                                                                   datasetMetadata(utils::dataset::getDatasetMetadata(datasetPath.parent_path() / "metadata.json")),
                                                                   timeline(sceneModel),
                                                                   viewContext(sceneCamera),
                                                                   annotationView(sceneModel, viewId),
                                                                   sceneMeshView(sceneModel.getMesh(), viewId),
                                                                   pointCloudView(sceneModel, viewId),
                                                                   lookatControl(viewId),
                                                                   addKeypointView(sceneModel, timeline, viewId),
                                                                   moveToolView(sceneModel, timeline, viewId),
                                                                   addBBoxView(sceneModel, datasetMetadata, timeline, viewId),
                                                                   addRectangleView(sceneModel, timeline, viewId),
                                                                   statusBarView(sceneModel, IdFactory::getInstance().getId()) {
  sceneModel.setPointCloudPath((datasetPath / "scene" / "cloud.ply").string());
  preview = std::make_shared<controllers::PreviewController>(sceneModel, datasetPath, IdFactory::getInstance().getId());
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

void StudioViewController::render(InputModifier mod) const {
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

  if (cameraControls.active()) {
    lookatControl.render(viewContext);
  }
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
  return cameraControls.leftButtonDown(viewContext);
}

bool StudioViewController::leftButtonUp(double x, double y, InputModifier mod) {
  updateViewContext(x, y, mod);

  if (preview->leftButtonUp(viewContext)) {
    return true;
  } else if (!cameraControls.leftButtonUp(viewContext)) {
    return getActiveToolView().leftButtonUp(viewContext);
  }
  return false;
}

bool StudioViewController::mouseMoved(double x, double y, InputModifier mod) {
  updateViewContext(x, y, mod);

  if (getActiveToolView().mouseMoved(viewContext)) {
    return true;
  }
  return cameraControls.mouseMoved(viewContext);
}

bool StudioViewController::scroll(double xoffset, double yoffset, InputModifier mod) {
  return cameraControls.scroll(xoffset, yoffset, viewContext);
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
    return true;
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
    if (integerValue < datasetMetadata.numClasses) {
      sceneModel.currentClassId = integerValue;
      getActiveToolView().keypress(character, mod);
    }
  }
  return false;
}

views::Rect StudioViewController::previewRect() const {
  float aspectRatio = float(sceneCamera.imageHeight) / float(sceneCamera.imageWidth);
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
  auto point = sceneModel.traceRay(viewContext.camera.getPosition(), rayDirection);
  if (point.has_value()) {
    viewContext.pointingAt = point;
  } else {
    viewContext.pointingAt = {};
  }
}

void StudioViewController::save() const {
  sceneModel.save(datasetPath / "annotations.json");
}

void StudioViewController::load() {
  timeline.load(datasetPath / "annotations.json");
}

void StudioViewController::undo() {
  timeline.undoCommand();
  refresh();
}
