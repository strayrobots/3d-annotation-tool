#pragma once
#include "views/view.h"
#include "views/bbox.h"
#include "views/mesh_view.h"
#include "views/rectangle_view.h"
#include "scene_model.h"

namespace views {

class AnnotationView : public views::View3D {
private:
  const SceneModel& sceneModel;
  views::MeshDrawable sphereDrawable;
  BBoxView bboxView;
  RectangleView rectangleView;

  Vector4f lightDir = Vector4f(0.0, 1.0, -1.0, 1.0);
public:
  AnnotationView(const SceneModel& model, int viewId = -1);
  void render(const ViewContext3D& context) const;
};
} // namespace views
