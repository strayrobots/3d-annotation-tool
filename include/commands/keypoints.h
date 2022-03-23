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

class ChangeKeypointClassCommand : public Command {
private:
  const Keypoint keypoint;
  const int newClassId;

public:
  ChangeKeypointClassCommand(const Keypoint kp, int newClassId);
  void execute(SceneModel& sceneModel) override;
  void undo(SceneModel& sceneModel) override;
};

} // namespace commands
