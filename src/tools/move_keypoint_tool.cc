#include "tools/move_keypoint_tool.h"
#include "controllers/studio_view_controller.h"
#include "controllers/annotation_controller.h"
#include "commands/move_keypoint_command.h"

using namespace Eigen;
using namespace commands;
using namespace tools;

namespace tools {
MoveKeypointTool::MoveKeypointTool(SceneModel& model, StudioViewController& c,
                                   controllers::AnnotationController& annotation, Timeline& timeline) : Tool(model, timeline),
                                                                                                         studioController(c), annotationController(annotation), rtKeypointSphere(std::make_shared<geometry::Sphere>(Matrix4f::Identity(), 0.01)) {
  translateControl = std::make_shared<views::controls::TranslateControl>([&](const Vector3f& newPosition) {
    newValue = newPosition;
    annotationController.setKeypointPosition(currentKeypoint, newPosition);
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
      annotationController.addControl(translateControl);
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

void MoveKeypointTool::activate() {
  studioController.meshView->setAlpha(0.35);
}

void MoveKeypointTool::deactivate() {
  active = false;
  annotationController.removeControl(translateControl);
  studioController.meshView->setAlpha(1.0);
}
}; // namespace tools
