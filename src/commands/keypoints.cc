#include "commands/keypoints.h"
#include "geometry/mesh.h"

namespace commands {

AddKeypointCommand::AddKeypointCommand(const Vector3f& p) : position(p) {
}
AddKeypointCommand::~AddKeypointCommand() {}

void AddKeypointCommand::execute(SceneModel& sceneModel) {
  keypoint = sceneModel.addKeypoint(position);
}

void AddKeypointCommand::undo(SceneModel& sceneModel) {
  sceneModel.removeKeypoint(keypoint);
}
} // namespace commands
