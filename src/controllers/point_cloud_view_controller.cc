#include <iostream>
#include <cassert>
#include "controllers/point_cloud_view_controller.h"
#include "commands/keypoints.h"
#include "id.h"
#include "utils/dataset.h"

using namespace commands;
using namespace views;
namespace fs = std::filesystem;

PointCloudViewController::PointCloudViewController(fs::path pointCloudPath) : viewId(IdFactory::getInstance().getId()),
                                                                              timeline(sceneModel),
                                                                              dataPath(pointCloudPath),
                                                                              sceneModel(),
                                                                              datasetMetadata(utils::dataset::getDatasetMetadata(dataPath.parent_path() / "metadata.json")),
                                                                              viewContext(),
                                                                              annotationView(sceneModel, viewId),
                                                                              pointCloudView(sceneModel, viewId),
                                                                              lookatControl(viewId),
                                                                              addKeypointView(sceneModel, timeline, viewId),
                                                                              moveToolView(sceneModel, timeline, viewId),
                                                                              addBBoxView(sceneModel, datasetMetadata, timeline, viewId),
                                                                              addRectangleView(sceneModel, timeline, viewId),
                                                                              statusBarView(sceneModel, IdFactory::getInstance().getId()) {

  annotationPath = utils::dataset::getAnnotationPathForPointCloudPath(dataPath);
  sceneModel.setPointCloudPath(dataPath); // TODO: Load a new path/cloud when tab is pressed
  pointCloudView.loadPointCloud();
  sceneModel.activeView = active_view::PointCloudView;
}

void PointCloudViewController::viewWillAppear(const views::Rect& rect) {
  pclMean = sceneModel.getPointCloud()->getMean();
  pclScale = sceneModel.getPointCloud()->getStd().norm();

  cameraControls.setSceneScale(pclScale);
  viewContext.camera.reset(pclMean, pclMean);
  viewContext.camera.zoom(-5 * pclScale); // TODO: Is there a better strategy? Seems to be fine for bot large and small scenes

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

void PointCloudViewController::render(InputModifier mod) const {
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

  if (mod & ModCommand) {
    lookatControl.render(viewContext);
  }
}

// Input handling.
bool PointCloudViewController::leftButtonDown(double x, double y, InputModifier mod) {
  updateViewContext(x, y, mod);

  if (getActiveToolView().leftButtonDown(viewContext)) {
    return true;
  }
  return cameraControls.leftButtonDown(viewContext);
}

bool PointCloudViewController::leftButtonUp(double x, double y, InputModifier mod) {
  updateViewContext(x, y, mod);

  if (!cameraControls.leftButtonUp(viewContext)) {
    return getActiveToolView().leftButtonUp(viewContext);
  }
  return false;
}

bool PointCloudViewController::mouseMoved(double x, double y, InputModifier mod) {
  updateViewContext(x, y, mod);

  if (getActiveToolView().mouseMoved(viewContext)) {
    return true;
  }

  if (cameraControls.mouseMoved(viewContext)) {
    return true;
  }
  return false;
}

bool PointCloudViewController::scroll(double xoffset, double yoffset, InputModifier mod) {
  cameraControls.scroll(xoffset, yoffset, viewContext);
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
    if (mod & ModCommand && (character == '+' || character == '=')) {
      pointCloudView.changeSize(1.0f);
      return true;
    } else if (mod & ModCommand && character == '-') {
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
      sceneModel.currentClassId = integerValue;
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
  } else {
    viewContext.pointingAt = {};
  }
}

void PointCloudViewController::save() const {
  // TODO: Modify file name according to current point cloud file and not the "root"
  sceneModel.save(annotationPath);
};

void PointCloudViewController::load() {
  // TODO: Modify file name according to current point cloud file and not the "root"
  timeline.load(annotationPath);
};

void PointCloudViewController::undo() {
  timeline.undoCommand();
  refresh();
}
