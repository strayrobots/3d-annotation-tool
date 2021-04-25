#pragma once
#include <iostream>
#include <optional>
#include "scene_model.h"
#include "tools/tool.h"
#include "commands/command.h"
#include "commands/keypoints.h"

using namespace Eigen;
using namespace commands;
using namespace tools;

namespace tools {
class AddKeypointTool : public Tool {
private:
  StudioViewController& studioViewController;
  std::optional<Vector3f> pointingAt;
public:
  AddKeypointTool(SceneModel& model, StudioViewController& c, CommandStack& stack);
  bool leftButtonUp(const ViewContext3D& viewContext) override;
  bool mouseMoved(const ViewContext3D& viewContext) override;
};
} // namespace tools
