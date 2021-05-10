#pragma once
#include <eigen3/Eigen/Dense>
#include "commands/command.h"
#include "scene_model.h"

namespace commands {

using namespace Eigen;
class AddKeypointCommand : public Command {
private:
  const Vector3f& position;
  Keypoint keypoint = {-1, Vector3f(0, 0, 0)};

public:
  AddKeypointCommand(const Vector3f& k);
  ~AddKeypointCommand() override;

  void execute(SceneModel& sceneModel) override;
  void undo(SceneModel& sceneModel) override;
};

} // namespace commands
