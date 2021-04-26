#include <iostream>
#include "commands/move_keypoint_command.h"
#include "controllers/studio_view_controller.h"

namespace commands {

MoveKeypointCommand::MoveKeypointCommand(Keypoint kp, const Vector3f& newPosition) : oldKeypoint(kp),
                                                                                     newKeypoint(kp.id, newPosition) {
}

void MoveKeypointCommand::execute(StudioViewController& view, SceneModel& sceneModel) {
  sceneModel.updateKeypoint(oldKeypoint.id, newKeypoint);
  view.annotationController.setKeypointPosition(oldKeypoint, newKeypoint.position);
}

void MoveKeypointCommand::undo(StudioViewController& view, SceneModel& sceneModel) {
  sceneModel.updateKeypoint(oldKeypoint.id, oldKeypoint);
  view.annotationController.setKeypointPosition(oldKeypoint, oldKeypoint.position);
}
}; // namespace commands
