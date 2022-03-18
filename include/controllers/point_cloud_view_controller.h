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
#include "timeline.h"

#include <memory>
#include <filesystem>
#include <fstream>
#include <bgfx/bgfx.h>
#include "3rdparty/json.hpp"
#include "commands/keypoints.h"
#include "commands/bounding_box.h"
#include "commands/rectangle.h"
#include "utils/serialize.h"

using namespace commands;

class PointCloudViewController : public controllers::Controller {
private:
  int viewId;
  SceneModel& sceneModel;
  std::pair<int, int> imageSize;

  // Changing view point.
  double prevX, prevY;
  bool dragging = false, moved = false;

  ViewContext3D viewContext;
  views::AnnotationView annotationView;
  views::PointCloudView pointCloudView;

  // Tool views.
  views::AddKeypointView addKeypointView;
  views::MoveToolView moveToolView;
  views::AddBBoxView addBBoxView;
  views::AddRectangleView addRectangleView;

  views::StatusBarView statusBarView;

public:
  PointCloudViewController(SceneModel& model, const std::string& folder);
  void viewWillAppear(const views::Rect& r) override;

  void render() const;
  void refresh();

  bool leftButtonDown(double x, double y, InputModifier mod);
  bool leftButtonUp(double x, double y, InputModifier mod);
  bool mouseMoved(double x, double y, InputModifier mod);
  bool scroll(double xoffset, double yoffset, InputModifier mod);
  bool keypress(char character, const InputModifier mod) override;
  void resize(const views::Rect& r) override;

  Timeline timeline;
  const std::filesystem::path datasetFolder;

  void loadState() {
    std::filesystem::path annotationPath(datasetFolder / "annotations.json");
    if (!std::filesystem::exists(annotationPath))
      return;
    std::ifstream file(annotationPath.string());
    nlohmann::json json;
    file >> json;
    for (auto& keypoint : json["keypoints"]) {
      auto position = keypoint["position"];
      auto k = Vector3f(position[0].get<float>(), position[1].get<float>(), position[2].get<float>());
      Keypoint kp(-1, keypoint["instance_id"].get<int>(), k);
      std::unique_ptr<Command> command = std::make_unique<commands::AddKeypointCommand>(kp);
      timeline.pushCommand(std::move(command));
    }
    for (auto& bbox : json["bounding_boxes"]) {
      auto p = bbox["position"];
      auto orn = bbox["orientation"];
      auto d = bbox["dimensions"];
      auto instanceId = bbox["instance_id"];
      BBox box = {
          .instanceId = instanceId,
          .position = utils::serialize::toVector3(p),
          .orientation = Quaternionf(orn["w"].get<float>(), orn["x"].get<float>(), orn["y"].get<float>(), orn["z"].get<float>()),
          .dimensions = utils::serialize::toVector3(d),
      };
      std::unique_ptr<Command> command = std::make_unique<commands::AddBBoxCommand>(box);
      timeline.pushCommand(std::move(command));
    }
    for (auto& rectangle : json["rectangles"]) {
      Rectangle rect(0, rectangle["class_id"],
                     utils::serialize::toVector3(rectangle["center"]),
                     utils::serialize::toQuaternion(rectangle["orientation"]),
                     utils::serialize::toVector2(rectangle["size"]));
      std::unique_ptr<Command> command = std::make_unique<commands::AddRectangleCommand>(rect);
      timeline.pushCommand(std::move(command));
    }
  }

private:
  views::View3D& getActiveToolView();
  views::Rect statusBarRect() const;
  void updateViewContext(double x, double y, InputModifier mod);
};
