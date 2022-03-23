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

ChangeKeypointClassCommand::ChangeKeypointClassCommand(const Keypoint kp, int newId) : keypoint(kp), newClassId(newId) {}

void ChangeKeypointClassCommand::execute(SceneModel& sceneModel) {
  Keypoint kp = keypoint;
  kp.classId = newClassId;
  sceneModel.updateKeypoint(kp.id, kp);
}

void ChangeKeypointClassCommand::undo(SceneModel& sceneModel) {
  sceneModel.updateKeypoint(keypoint.id, keypoint);
}
} // namespace commands
