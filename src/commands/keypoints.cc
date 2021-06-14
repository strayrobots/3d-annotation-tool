#include "commands/keypoints.h"
#include "geometry/mesh.h"

namespace commands {

AddKeypointCommand::AddKeypointCommand(const Keypoint& p) : keypoint(p) {
}
AddKeypointCommand::~AddKeypointCommand() {}

void AddKeypointCommand::execute(SceneModel& sceneModel) {
  keypoint = sceneModel.addKeypoint(keypoint);
}

void AddKeypointCommand::undo(SceneModel& sceneModel) {
  sceneModel.removeKeypoint(keypoint);
}

ChangeKeypointInstanceIdCommand::ChangeKeypointInstanceIdCommand(const Keypoint kp, int newId) : keypoint(kp), newInstanceId(newId) {}

void ChangeKeypointInstanceIdCommand::execute(SceneModel& sceneModel) {
  Keypoint kp = keypoint;
  kp.instanceId = newInstanceId;
  sceneModel.updateKeypoint(kp.id, kp);
}

void ChangeKeypointInstanceIdCommand::undo(SceneModel& sceneModel) {
  sceneModel.updateKeypoint(keypoint.id, keypoint);
}
} // namespace commands
