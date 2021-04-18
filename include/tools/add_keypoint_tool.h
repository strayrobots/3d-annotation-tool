#pragma once
#include <iostream>
#include "tools/tool.h"
#include "commands/command.h"
#include "commands/keypoints.h"
#include "scene_model.h"

using namespace Eigen;
using namespace commands;
using namespace tools;

namespace tools {
class AddKeypointTool : public Tool {
private:
public:
  AddKeypointTool(const SceneModel& model) : Tool(model) {

  }

  std::optional<std::unique_ptr<Command>> leftClick(const std::optional<Vector3f>& pointingAt) override {
    if (!pointingAt.has_value()) return {};
    std::unique_ptr<Command> command = std::make_unique<AddKeypointCommand>(pointingAt.value());
    std::cout << "Added keypoint: " << pointingAt.value().transpose() << std::endl;
    return command;
  }
};
}

