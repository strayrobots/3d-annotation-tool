#pragma once
#include <eigen3/Eigen/Dense>
#include "commands/command.h"
#include "scene_model.h"

namespace commands {

using namespace Eigen;
class AddKeypointCommand : public Command {
private:
  Keypoint keypoint;
public:
  AddKeypointCommand(const Keypoint& keypoint);
  ~AddKeypointCommand() override;

  void execute(SceneModel& sceneModel) override;
  void undo(SceneModel& sceneModel) override;
};

class ChangeKeypointInstanceIdCommand : public Command {
private:
  const Keypoint keypoint;
  const int newInstanceId;
public:
 ChangeKeypointInstanceIdCommand(const Keypoint kp, int newInstanceId);
 void execute(SceneModel& sceneModel) override;
 void undo(SceneModel& sceneModel) override;
};

} // namespace commands
