#include "commands/move_keypoint_command.h"
#include "controllers/studio_view_controller.h"

namespace commands {

MoveKeypointCommand::MoveKeypointCommand(Keypoint kp, const Vector3f& newPosition) : oldKeypoint(kp),
                                                                                     newKeypoint(kp.id, newPosition) {
}

void MoveKeypointCommand::execute(SceneModel& sceneModel) {
  sceneModel.updateKeypoint(oldKeypoint.id, newKeypoint);
}

void MoveKeypointCommand::undo(SceneModel& sceneModel) {
  sceneModel.updateKeypoint(oldKeypoint.id, oldKeypoint);
}
}; // namespace commands
