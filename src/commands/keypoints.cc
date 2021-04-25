#include "commands/keypoints.h"
#include "geometry/mesh.h"
#include "controllers/studio_view_controller.h"

namespace commands {

AddKeypointCommand::AddKeypointCommand(const Vector3f& k) : keypoint(k) {
  Matrix4f T = Matrix4f::Identity();
  T.block(0, 3, 3, 1) = keypoint;
  sphere = std::make_shared<geometry::Sphere>(T, 0.01);
}
AddKeypointCommand::~AddKeypointCommand() {}

void AddKeypointCommand::execute(StudioViewController& view, SceneModel& sceneModel) {
  auto keypoints = sceneModel.getKeypoints();
  keypoints.push_back(keypoint);
  sceneModel.setKeypoints(keypoints);
  auto sphereDrawable = std::make_shared<views::MeshDrawable>(sphere, KeypointColor);
  view.meshView->addObject(sphereDrawable);
}

void AddKeypointCommand::undo(StudioViewController& view, SceneModel& sceneModel) {
  auto keypoints = sceneModel.getKeypoints();
  keypoints.pop_back();
  sceneModel.setKeypoints(keypoints);
  view.meshView->popObject();
}
} // namespace commands
