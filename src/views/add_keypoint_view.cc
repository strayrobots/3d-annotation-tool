#include "views/add_keypoint_view.h"
#include "commands/keypoints.h"

namespace views {
using namespace commands;

AddKeypointView::AddKeypointView(SceneModel& model, Timeline& timeline, int viewId) : views::View3D(viewId), sceneModel(model), timeline(timeline) {}

bool AddKeypointView::leftButtonUp(const ViewContext3D& viewContext) {
  if (!pointingAt.has_value()) return false;

  Keypoint kp(-1, sceneModel.currentInstanceId, pointingAt.value());
  auto command = std::make_unique<commands::AddKeypointCommand>(kp);
  timeline.pushCommand(std::move(command));
  return true;
}

bool AddKeypointView::mouseMoved(const ViewContext3D& viewContext) {
  const Vector3f& rayDirection = viewContext.camera.computeRayWorld(viewContext.width, viewContext.height,
                                                                    viewContext.mousePositionX, viewContext.mousePositionY);
  pointingAt = sceneModel.traceRay(viewContext.camera.getPosition(), rayDirection);
  return false;
}
} // namespace views
