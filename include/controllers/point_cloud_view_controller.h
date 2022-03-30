#pragma once
#include <memory>
#include "timeline.h"
#include "views/view.h"
#include "scene_model.h"
#include "view_context_3d.h"
#include "controllers/controller.h"
#include "views/point_cloud_view.h"
#include "views/annotation_view.h"
#include "views/add_keypoint_view.h"
#include "views/move_tool_view.h"
#include "views/add_bbox_view.h"
#include "views/status_bar_view.h"
#include "views/add_rectangle_view.h"
#include <bgfx/platform.h>
#include "glfw_app.h"
#include "scene_model.h"
#include <filesystem>
#include <fstream>
#include <bgfx/bgfx.h>
#include "3rdparty/json.hpp"
#include "commands/keypoints.h"
#include "commands/bounding_box.h"
#include "commands/rectangle.h"
#include "utils/serialize.h"
#include "views/controls/lookat.h"
#include "camera/camera_controls.h"

using namespace commands;
namespace fs = std::filesystem;
class PointCloudViewController : public controllers::Controller {
private:
  int viewId;
  Timeline timeline;
  fs::path dataPath;
  fs::path annotationPath;
  SceneModel sceneModel;
  DatasetMetadata datasetMetadata;

  Vector3f pclMean;
  float pclScale;

  ViewContext3D viewContext;
  views::AnnotationView annotationView;
  views::PointCloudView pointCloudView;
  views::controls::LookatControl lookatControl;

  // Tool views.
  views::AddKeypointView addKeypointView;
  views::MoveToolView moveToolView;
  views::AddBBoxView addBBoxView;
  views::AddRectangleView addRectangleView;

  views::StatusBarView statusBarView;

  camera::CameraControls cameraControls;

public:
  PointCloudViewController(fs::path folder);
  void viewWillAppear(const views::Rect& r) override;

  void render(InputModifier mod) const;
  void refresh();

  bool leftButtonDown(double x, double y, InputModifier mod);
  bool leftButtonUp(double x, double y, InputModifier mod);
  bool mouseMoved(double x, double y, InputModifier mod);
  bool scroll(double xoffset, double yoffset, InputModifier mod);
  bool keypress(char character, const InputModifier mod) override;
  void resize(const views::Rect& r) override;

  const std::filesystem::path datasetFolder;

  void save() const;
  void load();
  void undo();

private:
  views::View3D& getActiveToolView();
  views::Rect statusBarRect() const;
  void updateViewContext(double x, double y, InputModifier mod);
};
