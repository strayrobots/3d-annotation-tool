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
  std::shared_ptr<controllers::AnnotationController> annotationController;
public:
  MoveKeypointTool(const SceneModel& model, std::shared_ptr<views::MeshView> mesh,
      std::shared_ptr<controllers::AnnotationController> annotation) : Tool(model) {
    meshView = mesh;
    annotationController = annotation;
  }

  void activate() override {
    meshView->setAlpha(0.35);
  }

  void deactivate() override {
    meshView->setAlpha(1.0);
  }

  std::optional<std::unique_ptr<Command>> leftClick(const std::optional<Vector3f>& pointingAt) override {
    return {};
  }
};
}

