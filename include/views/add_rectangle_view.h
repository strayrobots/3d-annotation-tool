#pragma once
#include <memory>
#include <optional>
#include <vector>
#include "views/view.h"
#include "views/point_view.h"
#include "views/rectangle_view.h"
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
  std::optional<Vector3f> pointingAt;
  std::optional<Vector3f> pointingAtNormal;
  std::vector<Vector3f> rectangleCorners;
  std::vector<Vector3f> normals;

  views::PointView pointView;
  views::RectangleView rectangleView;
  std::array<Vector3f, 4> vertices;
public:
  AddRectangleView(SceneModel& model, Timeline& timeline, int viewId = 0);
  bool leftButtonUp(const ViewContext3D& viewContext) override;
  bool leftButtonDown(const ViewContext3D& context) override;
  bool mouseMoved(const ViewContext3D& context) override;
  void render(const ViewContext3D& context) const;
private:
  bool hasRectangle() const;
  void computeVertices();
  void commit();
};
} // namespace views
