#pragma once
#include <eigen3/Eigen/Dense>
#include "commands/command.h"
#include "scene_model.h"
#include "views/controls/translate.h"

class StudioViewController;

namespace commands {

using namespace Eigen;
class MoveKeypointCommand : public Command {
private:
  Keypoint oldKeypoint;
  Keypoint newKeypoint;
public:
  MoveKeypointCommand(Keypoint kp, const Vector3f& newPosition);

  void execute(StudioViewController& view, SceneModel& sceneModel) override;
  void undo(StudioViewController& view, SceneModel& sceneModel) override;
};
}
