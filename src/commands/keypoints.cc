#include "commands/keypoints.h"
#include "geometry/mesh.h"
#include "controllers/studio_view_controller.h"

namespace commands {

AddKeypointCommand::AddKeypointCommand(const Vector3f& k) : keypoint(k) {
}
AddKeypointCommand::~AddKeypointCommand() {}

void AddKeypointCommand::execute(StudioViewController& view, SceneModel& sceneModel) {
  auto keypoints = sceneModel.getKeypoints();
  keypoints.push_back(keypoint);
  sceneModel.setKeypoints(keypoints);
  view.annotationController->addKeypoint(keypoint);
}

void AddKeypointCommand::undo(StudioViewController& view, SceneModel& sceneModel) {
  auto keypoints = sceneModel.getKeypoints();
  keypoints.pop_back();
  sceneModel.setKeypoints(keypoints);
  view.annotationController->removeKeypoint(keypoint);
}
}
