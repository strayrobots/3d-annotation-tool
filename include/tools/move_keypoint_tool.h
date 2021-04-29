#pragma once
#include <iostream>
#include <list>
#include "timeline.h"
#include "tools/tool.h"
#include "commands/command.h"
#include "scene_model.h"
#include "views/controls/translate.h"

using namespace Eigen;
using namespace commands;
using namespace tools;

namespace tools {
class MoveKeypointTool : public Tool {
private:
  Keypoint currentKeypoint = Keypoint(-1);
  Vector3f newValue = Vector3f::Zero();
  std::shared_ptr<views::controls::TranslateControl> translateControl;
  geometry::RayTraceMesh rtKeypointSphere;
  bool active = false;

public:
  MoveKeypointTool(SceneModel& model, Timeline& tl);
  bool leftButtonUp(const ViewContext3D& context) override;
  bool leftButtonDown(const ViewContext3D& context) override;
  bool mouseMoved(const ViewContext3D& context) override;
};
} // namespace tools
