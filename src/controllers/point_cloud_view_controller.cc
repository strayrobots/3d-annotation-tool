#include <iostream>
#include <cassert>
#include "controllers/point_cloud_view_controller.h"
#include "commands/keypoints.h"
#include "id.h"
#include "utils/dataset.h"

using namespace commands;
using namespace views;
namespace fs = std::filesystem;

PointCloudViewController::PointCloudViewController(fs::path folder) : viewId(IdFactory::getInstance().getId()),
                                                                      timeline(sceneModel),
                                                                      datasetPath(folder),
                                                                      sceneModel(std::nullopt),
                                                                      datasetMetadata(utils::dataset::getDatasetMetadata(datasetPath / "metadata.json")),
                                                                      pointCloudPaths(utils::dataset::getDatasetPointCloudPaths(folder)),
                                                                      viewContext(),
                                                                      annotationView(sceneModel, viewId),
                                                                      pointCloudView(sceneModel, viewId),
                                                                      addKeypointView(sceneModel, timeline, viewId),
                                                                      moveToolView(sceneModel, timeline, viewId),
                                                                      addBBoxView(sceneModel, datasetMetadata, timeline, viewId),
                                                                      addRectangleView(sceneModel, timeline, viewId),
                                                                      statusBarView(sceneModel, IdFactory::getInstance().getId()) {

  sceneModel.setPointCloudPath(pointCloudPaths[0]); // TODO: Load a new path/cloud when tab is pressed
  pointCloudView.loadPointCloud();
  sceneModel.activeView = active_view::PointCloudView;
}

void PointCloudViewController::viewWillAppear(const views::Rect& rect) {
  viewContext.camera.reset(Vector3f::UnitZ(), Vector3f::Zero());

  viewContext.width = rect.width;
  viewContext.height = rect.height - views::StatusBarHeight;

  bgfx::setViewClear(viewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);
  statusBarView.setRect(statusBarRect());
}

views::View3D& PointCloudViewController::getActiveToolView() {
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

void PointCloudViewController::refresh() {
  addBBoxView.refresh();
  moveToolView.refresh();
}

void PointCloudViewController::render() const {
  bgfx::setViewRect(viewId, 0, 0, viewContext.width, viewContext.height);
  annotationView.render(viewContext);

  if (sceneModel.activeToolId == MoveKeypointToolId) {
    moveToolView.render(viewContext);
  } else if (sceneModel.activeToolId == BBoxToolId) {
    addBBoxView.render(viewContext);
  } else if (sceneModel.activeToolId == AddRectangleToolId) {
    addRectangleView.render(viewContext);
  }

  pointCloudView.render(viewContext);
  statusBarView.render();
}

// Input handling.
bool PointCloudViewController::leftButtonDown(double x, double y, InputModifier mod) {
  updateViewContext(x, y, mod);

  if (getActiveToolView().leftButtonDown(viewContext)) {
    return true;
  }
  dragging = true;
  moved = false;
  prevX = x;
  prevY = y;
  return true;
}

bool PointCloudViewController::leftButtonUp(double x, double y, InputModifier mod) {
  updateViewContext(x, y, mod);

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

bool PointCloudViewController::mouseMoved(double x, double y, InputModifier mod) {
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

bool PointCloudViewController::scroll(double xoffset, double yoffset, InputModifier mod) {
  float diff = yoffset * 0.05;
  viewContext.camera.zoom(diff);
  return true;
}

void PointCloudViewController::resize(const views::Rect& rect) {
  viewContext.width = rect.width;
  viewContext.height = rect.height - views::StatusBarHeight;
  statusBarView.setRect(statusBarRect());
}

bool PointCloudViewController::keypress(char character, const InputModifier mod) {
  // TODO: Load a new path/cloud when tab is pressed
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
  if (character == 'K') {
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
      sceneModel.currentInstanceId = integerValue;
      getActiveToolView().keypress(character, mod);
    }
  }
  return false;
}

views::Rect PointCloudViewController::statusBarRect() const {
  return {.x = 0, .y = float(viewContext.height), .width = float(viewContext.width), .height = float(views::StatusBarHeight)};
}

void PointCloudViewController::updateViewContext(double x, double y, InputModifier mod) {
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

void PointCloudViewController::save() const {
  // TODO: Modify file name according to current point cloud file and not the "root"
  sceneModel.save(datasetPath / "annotations.json");
};

void PointCloudViewController::load() {
  // TODO: Modify file name according to current point cloud file and not the "root"
  timeline.load(datasetPath / "annotations.json");
};

void PointCloudViewController::undo() {
  timeline.undoCommand();
  refresh();
}
