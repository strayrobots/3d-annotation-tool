#ifndef H_KEYPOINT_COMMAND
#define H_KEYPOINT_COMMAND
#include <eigen3/Eigen/Dense>
#include "commands/command.h"
#include "studio_view.h"
#include "scene_model.h"

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

  void execute(StudioView& view, SceneModel& sceneModel) override;
  void undo(StudioView& view, SceneModel& sceneModel) override;
};
}
#endif
