#include "commands/move_keypoint_command.h"

namespace commands {

MoveKeypointCommand::MoveKeypointCommand(const Keypoint& kp, const Vector3f& newPosition) : oldKeypoint(kp),
                                                                                     newKeypoint(kp.id, newPosition) {
}

void MoveKeypointCommand::execute(SceneModel& sceneModel) {
  sceneModel.updateKeypoint(oldKeypoint.id, newKeypoint);
}

void MoveKeypointCommand::undo(SceneModel& sceneModel) {
  sceneModel.updateKeypoint(oldKeypoint.id, oldKeypoint);
  sceneModel.activeKeypoint = -1;
}
}; // namespace commands
