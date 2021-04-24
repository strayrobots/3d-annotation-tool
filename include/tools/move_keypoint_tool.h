#pragma once
#include <iostream>
#include <list>
#include "tools/tool.h"
#include "commands/command.h"
#include "scene_model.h"
#include "controllers/annotation_controller.h"

using namespace Eigen;
using namespace commands;
using namespace tools;

namespace tools {
class MoveKeypointTool : public Tool {
private:
  Keypoint currentKeypoint = Keypoint(-1);
  Vector3f newValue = Vector3f::Zero();
  StudioViewController& studioController;
  controllers::AnnotationController& annotationController;
  std::shared_ptr<views::controls::TranslateControl> translateControl;
  geometry::RayTraceMesh rtKeypointSphere;
  bool active = false;
public:
  MoveKeypointTool(SceneModel& model, StudioViewController& c,
      controllers::AnnotationController& annotation, CommandStack& stack);
  bool leftButtonUp(const ViewContext3D& context) override;
  bool leftButtonDown(const ViewContext3D& context) override;
  bool mouseMoved(const ViewContext3D& context) override;
  void activate() override;
  void deactivate() override;
};
}

