#pragma once
#include <memory>
#include "views/view.h"
#include "timeline.h"
#include "views/controls/translate.h"

namespace views {
class MoveKeypointView : public views::View3D {
private:
  SceneModel& sceneModel;
  Timeline& timeline;
  std::shared_ptr<views::controls::TranslateControl> translateControl;
  Vector3f newValue = Vector3f::Zero();
  geometry::RayTraceMesh rtKeypointSphere;
  std::optional<Keypoint> currentKeypoint = {};
  bool dragging = false;

public:
  MoveKeypointView(SceneModel& model, Timeline& tl);
  bool isActive() const;
  bool leftButtonUp(const ViewContext3D& context) override;
  bool leftButtonDown(const ViewContext3D& context) override;
  bool mouseMoved(const ViewContext3D& context) override;
  void render(const ViewContext3D& context) const;
};
} // namespace views
