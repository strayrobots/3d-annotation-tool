#ifndef H_KEYPOINT_COMMAND
#define H_KEYPOINT_COMMAND
#include <eigen3/Eigen/Dense>
#include "commands/command.h"
#include "scene_model.h"

class StudioViewController;

namespace commands {

const Vector4f KeypointColor(1.0, 0.5, 0.5, 1.0);

using namespace Eigen;
class AddKeypointCommand : public Command {
private:
  const Vector3f& keypoint;
  std::shared_ptr<geometry::Sphere> sphere;

public:
  AddKeypointCommand(const Vector3f& k);
  ~AddKeypointCommand() override;

  void execute(StudioViewController& view, SceneModel& sceneModel) override;
  void undo(StudioViewController& view, SceneModel& sceneModel) override;
};
} // namespace commands
#endif
