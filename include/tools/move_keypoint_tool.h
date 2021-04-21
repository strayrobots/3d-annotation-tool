#pragma once
#include <iostream>
#include <list>
#include "tools/tool.h"
#include "commands/command.h"
#include "commands/keypoints.h"
#include "scene_model.h"
#include "controllers/studio_view_controller.h"
#include "controllers/annotation_controller.h"
#include "views/mesh_view.h"

using namespace Eigen;
using namespace commands;
using namespace tools;

namespace tools {
class MoveKeypointTool : public Tool {
private:
  std::shared_ptr<views::MeshView> meshView;
  controllers::AnnotationController& annotationController;
  std::shared_ptr<views::controls::TranslateControl> translateControl;
public:
  MoveKeypointTool(const SceneModel& model, std::shared_ptr<views::MeshView> mesh,
      controllers::AnnotationController& annotation) : Tool(model),
      annotationController(annotation) {
    meshView = mesh;
    translateControl = std::make_shared<views::controls::TranslateControl>();
  }

  void activate() override {
    meshView->setAlpha(0.35);
    annotationController.addControl(translateControl);
  }

  void deactivate() override {
    meshView->setAlpha(1.0);
    annotationController.removeControl(translateControl);
  }

  std::optional<std::unique_ptr<Command>> leftClick(const std::optional<Vector3f>& pointingAt) override {
    return {};
  }
};
}

