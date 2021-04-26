#pragma once
#include "commands/command.h"
#include "scene_model.h"

namespace commands {

using namespace Eigen;
class MoveKeypointCommand : public Command {
private:
  Keypoint oldKeypoint;
  Keypoint newKeypoint;

public:
  MoveKeypointCommand(Keypoint kp, const Vector3f& newPosition);

  void execute(SceneModel& sceneModel) override;
  void undo(SceneModel& sceneModel) override;
};
} // namespace commands
