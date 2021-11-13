#pragma once
#include <optional>
#include "commands/rectangle.h"
#include "views/view.h"
#include "timeline.h"
#include "scene_model.h"

namespace views {

using TMatrix = Eigen::Transform<float, 3, Eigen::Affine>;

class RectangleAffordances : public views::View3D {
private:
  struct Dragging {
    Rectangle oldRectangle;
    Rectangle newRectangle;
    Vector3f dragPoint_R; // drag point in local coordinates.
    bool copying = false;
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
  bool hitTest(const ViewContext3D& viewContext, const Rectangle& rectangle);
};

class AffordanceView : public views::View3D {
private:
  SceneModel& sceneModel;
  Timeline& timeline;
  RectangleAffordances rectangleAffordances;
public:
  AffordanceView(SceneModel& model, Timeline& timeline, int viewId);
  bool leftButtonDown(const ViewContext3D& viewContext);
  bool mouseMoved(const ViewContext3D& viewContext);
  bool leftButtonUp(const ViewContext3D& viewContext);
  void render(const ViewContext3D& context) const;
};

}

