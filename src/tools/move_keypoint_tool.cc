#include "tools/move_keypoint_tool.h"
#include "controllers/studio_view_controller.h"
#include "commands/move_keypoint_command.h"

using namespace Eigen;
using namespace commands;
using namespace tools;

namespace tools {
MoveKeypointTool::MoveKeypointTool(SceneModel& model, Timeline& timeline) :
    Tool(model, timeline), rtKeypointSphere(std::make_shared<geometry::Sphere>(Matrix4f::Identity(), 0.01)) {
  translateControl = std::make_shared<views::controls::TranslateControl>([&](const Vector3f& newPosition) {
    newValue = newPosition;
  });
}

bool MoveKeypointTool::leftButtonUp(const ViewContext3D& context) {
  if (active) {
    auto command = std::make_unique<MoveKeypointCommand>(currentKeypoint, newValue);
    currentKeypoint = Keypoint(currentKeypoint.id, newValue);
    timeline.pushCommand(std::move(command));
    active = false;
    return true;
  }

  return false;
}

bool MoveKeypointTool::leftButtonDown(const ViewContext3D& context) {
  if (translateControl->leftButtonDown(context)) {
    active = true;
  }
  for (const Keypoint& kp : sceneModel.getKeypoints()) {
    auto position = context.camera.getPosition() - kp.position;
    auto rayDirection = context.camera.computeRayWorld(context.width, context.height,
                                                       context.mousePositionX, context.mousePositionY);
    auto hit = rtKeypointSphere.traceRay(position, rayDirection);
    if (hit.has_value()) {
      currentKeypoint = kp;
      translateControl->setPosition(kp.position);
      return true;
    }
  }

  return false;
}

bool MoveKeypointTool::mouseMoved(const ViewContext3D& context) {
  if (active) {
    return translateControl->mouseMoved(context);
  } else {
    return false;
  }
}

}; // namespace tools
