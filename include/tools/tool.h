#pragma once
#include "scene_model.h"
#include "commands/command.h"

namespace tools {

class Tool {
private:
  const SceneModel& sceneModel;
public:
  Tool(const SceneModel& sceneModel);
  virtual ~Tool() {};
  virtual std::optional<std::unique_ptr<commands::Command>> leftClick(const std::optional<Vector3f>& pointingAt) = 0;
  void mouseMove();
};
}
