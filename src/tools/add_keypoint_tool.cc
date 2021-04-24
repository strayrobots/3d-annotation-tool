#include "tools/add_keypoint_tool.h"
#include "controllers/studio_view_controller.h"

namespace tools {
AddKeypointTool::AddKeypointTool(SceneModel& model, StudioViewController& c, CommandStack& stack) :
    Tool(model, stack), studioViewController(c) {
}

bool AddKeypointTool::leftButtonUp(const ViewContext3D& viewContext) {
  if (!pointingAt.has_value()) return false;

  std::unique_ptr<Command> command = std::make_unique<commands::AddKeypointCommand>(pointingAt.value());
  command->execute(studioViewController, sceneModel);
  commandStack.push_back(std::move(command));
  std::cout << "Added keypoint: " << pointingAt.value().transpose() << std::endl;
  return true;
}

bool AddKeypointTool::mouseMoved(const ViewContext3D& viewContext) {
  const Vector3f& rayDirection = viewContext.camera.computeRayWorld(viewContext.width, viewContext.height,
    viewContext.mousePositionX, viewContext.mousePositionY);
  pointingAt = sceneModel.traceRay(viewContext.camera.getPosition(), rayDirection);
  return false;
}

};
