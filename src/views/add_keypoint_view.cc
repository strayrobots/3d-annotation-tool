#include "views/add_keypoint_view.h"
#include "commands/keypoints.h"

namespace views {
using namespace commands;

AddKeypointView::AddKeypointView(SceneModel& model, Timeline& timeline) : sceneModel(model), timeline(timeline) {}

bool AddKeypointView::leftButtonUp(const ViewContext3D& viewContext) {
  if (!pointingAt.has_value()) return false;

  auto command = std::make_unique<commands::AddKeypointCommand>(pointingAt.value());
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
