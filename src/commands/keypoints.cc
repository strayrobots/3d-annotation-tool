#include "commands/keypoints.h"
#include "geometry/mesh.h"
#include "controllers/studio_view_controller.h"

namespace commands {

AddKeypointCommand::AddKeypointCommand(const Vector3f& p) : position(p) {
}
AddKeypointCommand::~AddKeypointCommand() {}

void AddKeypointCommand::execute(StudioViewController& view, SceneModel& sceneModel) {
  keypoint = sceneModel.addKeypoint(position);
  view.annotationController.addKeypoint(keypoint);
}

void AddKeypointCommand::undo(StudioViewController& view, SceneModel& sceneModel) {
  sceneModel.removeKeypoint(keypoint);
  view.annotationController.removeKeypoint(keypoint);
}
} // namespace commands
