#pragma once
#include <memory>
#include "views/view.h"
#include "timeline.h"
#include "views/affordance_view.h"
#include "views/controls/translate.h"

namespace views {
class MoveToolView : public views::View3D {
private:
  SceneModel& sceneModel;
  Timeline& timeline;
  std::shared_ptr<views::controls::TranslateControl> translateControl;
  Vector3f newValue = Vector3f::Zero();
  geometry::RayTraceMesh rtKeypointSphere;
  std::optional<Keypoint> currentKeypoint = {};
  bool dragging = false;

  views::AffordanceView affordanceView;
public:
  MoveToolView(SceneModel& model, Timeline& tl, int viewId = 0);
  bool isActive() const;
  bool leftButtonDown(const ViewContext3D& context) override;
  bool mouseMoved(const ViewContext3D& context) override;
  bool leftButtonUp(const ViewContext3D& context) override;
  bool keypress(const char character, const InputModifier& mod) override;
  void refresh();
  void render(const ViewContext3D& context) const;
};
} // namespace views
