#pragma once
#include <iostream>
#include <memory>
#include <optional>
#include <vector>
#include "views/view.h"
#include "scene_model.h"
#include "view_context_3d.h"
#include "timeline.h"

namespace views {

using namespace geometry;

class AddRectangleView : public views::View3D {
private:
  SceneModel& sceneModel;
  Timeline& timeline;
  std::vector<Eigen::Vector3f> points;
  Vector3f position = Vector3f::Zero();
  Quaternionf orientation;
  Vector2f dimensions = Vector2f::Zero();
  Vector3f normal = Vector3f::Zero();
  std::optional<Vector3f> pointingAt;

public:
  AddRectangleView(SceneModel& model, Timeline& timeline, int viewId = 0);
  bool leftButtonUp(const ViewContext3D& viewContext) override;
  bool leftButtonDown(const ViewContext3D& context) override;
  bool mouseMoved(const ViewContext3D& context) override;
  void render(const ViewContext3D& context) const;
};
} // namespace views
