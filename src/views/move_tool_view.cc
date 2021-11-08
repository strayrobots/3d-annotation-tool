#include <memory>
#include "views/move_tool_view.h"
#include "commands/keypoints.h"
#include "commands/move_keypoint_command.h"

namespace views {
MoveToolView::MoveToolView(SceneModel& model, Timeline& tl, int viewId) : views::View3D(viewId),
    sceneModel(model), timeline(tl),
    rtKeypointSphere(std::make_shared<geometry::Sphere>(Matrix4f::Identity(), 0.01)),
    affordanceView(sceneModel, timeline, viewId) {
  translateControl = std::make_shared<views::controls::TranslateControl>(viewId, [&](const Vector3f& newPosition) {
    if (!isActive()) return;
    auto kp = sceneModel.getKeypoint(currentKeypoint.value().id);
    if (kp.has_value()) {
      kp.value().position = newPosition;
      sceneModel.setKeypoint(kp.value());
    }
  });
}

bool MoveToolView::isActive() const {
  return currentKeypoint.has_value() && sceneModel.activeKeypoint == currentKeypoint.value().id;
}

bool MoveToolView::leftButtonDown(const ViewContext3D& context) {
  if (affordanceView.leftButtonDown(context)) {
    return true;
  }

  if (isActive() && translateControl->leftButtonDown(context)) {
    dragging = true;
    return true;
  }
  for (const Keypoint& kp : sceneModel.getKeypoints()) {
    auto position = context.camera.getPosition() - kp.position;
    auto rayDirection = context.camera.computeRayWorld(context.width, context.height,
                                                       context.mousePositionX, context.mousePositionY);
    auto hit = rtKeypointSphere.traceRay(position, rayDirection);
    if (hit.has_value()) {
      currentKeypoint = Keypoint(kp);
      translateControl->setPosition(kp.position);
      sceneModel.activeKeypoint = kp.id;
      return true;
    }
  }

  return false;
}

bool MoveToolView::mouseMoved(const ViewContext3D& context) {
  if (affordanceView.mouseMoved(context)) {
    return true;
  }
  if (isActive() && dragging) {
    return translateControl->mouseMoved(context);
  } else {
    return false;
  }
}

bool MoveToolView::leftButtonUp(const ViewContext3D& context) {
  if (affordanceView.leftButtonUp(context)) {
    return true;
  }
  if (dragging) {
    auto kp = sceneModel.getKeypoint(currentKeypoint.value().id);
    if (kp.has_value()) {
      auto command = std::make_unique<commands::MoveKeypointCommand>(currentKeypoint.value(), kp.value());
      timeline.pushCommand(std::move(command));
      currentKeypoint = kp.value();
    }
    dragging = false;
    return true;
  }

  return false;
}

void MoveToolView::refresh() {
  currentKeypoint = sceneModel.getKeypoint(sceneModel.activeKeypoint);
}

bool MoveToolView::keypress(const char character, const InputModifier& mod) {
  if ('0' <= character && character <= '9') {
    auto kp = currentKeypoint.value();
    if (kp.instanceId == character - '0') return true;
    auto command = std::make_unique<commands::ChangeKeypointInstanceIdCommand>(kp, sceneModel.currentInstanceId);
    timeline.pushCommand(std::move(command));
    currentKeypoint = sceneModel.getKeypoint(kp.id);
    return true;
  }
  return true;
}

void MoveToolView::render(const ViewContext3D& context) const {
  if (isActive()) {
    translateControl->render(context.camera);
  }
}
} // namespace views
