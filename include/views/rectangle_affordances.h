#pragma once
#include "commands/rectangle.h"
#include "views/view.h"
#include "timeline.h"
#include "scene_model.h"

namespace views {

class RectangleAffordances : public views::View3D {
private:
  enum HitType {
    None,
    Move,
    Rotate,
    Resize
  };
  struct Dragging {
    Rectangle oldRectangle;
    Rectangle newRectangle;
    Vector3f dragPoint_R; // drag point in local coordinates.
    bool copying = false;
    HitType dragType = HitType::None;
  };
  SceneModel& scene;
  Timeline& timeline;
  std::optional<Dragging> dragging;
public:
  RectangleAffordances(SceneModel& sceneModel, Timeline& tl);
  bool leftButtonDown(const ViewContext3D& viewContext);
  bool mouseMoved(const ViewContext3D& viewContext);
  bool leftButtonUp(const ViewContext3D& viewContext);

private:
  bool isActive() const;
  HitType hitTest(const ViewContext3D& viewContext, const Rectangle& rectangle);
};

}
