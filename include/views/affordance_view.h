#pragma once
#include <optional>
#include "views/view.h"
#include "scene_model.h"

namespace views {

using TMatrix = Eigen::Transform<float, 3, Eigen::Affine>;

class RectangleAffordances : public views::View3D {
private:
  struct Dragging {
    Rectangle rectangle;
    Vector3f dragPoint_R; // drag point in local coordinates.
    Vector3f originalCenter_W;
  };
  SceneModel& scene;
  std::optional<Dragging> dragging;
public:
  RectangleAffordances(SceneModel& sceneModel) : scene(sceneModel) {}
  bool leftButtonDown(const ViewContext3D& viewContext) {
    for (auto& rect : scene.getRectangles()) {
      if (hitTest(viewContext, rect)) {
        auto point = intersectionLocal(viewContext, rect);
        dragging = {
          .rectangle = rect,
          .dragPoint_R = point,
          .originalCenter_W = rect.center
        };
        return true;
      }
    }
    return false;
  }

  bool mouseMoved(const ViewContext3D& viewContext) {
    if (isActive()) {
      Dragging& d = dragging.value();
      Rectangle rect = d.rectangle;
      auto intersection_R = intersectionLocal(viewContext, rect);
      auto T_RW = computeT_RW(rect);
      Vector3f diff_R = intersection_R - d.dragPoint_R;
      rect.center = d.originalCenter_W + T_RW.rotation().transpose() * diff_R;
      scene.updateRectangle(rect);
      return true;
    }
    return false;
  }

  bool leftButtonUp(const ViewContext3D& viewContext) {
    if (isActive()) {
      dragging.reset();
      return true;
    }
    return false;
  }

private:
  bool isActive() const { return dragging.has_value(); }

  TMatrix computeT_RW(const Rectangle& rectangle) {
    TMatrix T_RW = TMatrix::Identity();
    T_RW.rotate(rectangle.orientation.inverse());
    T_RW.translate(-rectangle.center);
    return T_RW;
  }

  Vector3f intersectionLocal(const ViewContext3D& viewContext, const Rectangle& rectangle) {
    TMatrix T_RW = computeT_RW(rectangle);

    Vector3f o_W = viewContext.camera.getPosition();
    Vector3f o_R = T_RW * o_W;
    Vector3f ray_W = viewContext.rayWorld();
    // In local coordinates.
    Vector3f ray_R = T_RW.rotation() * ray_W;
    float t = -o_R[2] / ray_R[2];
    return o_R + t * ray_R;
  }

  bool hitTest(const ViewContext3D& viewContext, const Rectangle& rectangle) {
    TMatrix T_RW = computeT_RW(rectangle);

    Vector3f o_W = viewContext.camera.getPosition();
    Vector3f o_R = T_RW * o_W;
    Vector3f ray_W = viewContext.rayWorld();
    // In local coordinates.
    Vector3f ray_R = T_RW.rotation() * ray_W;
    float t = -o_R[2] / ray_R[2];
    Vector3f intersection_R = o_R + t * ray_R;
    float halfWidth = rectangle.width() * 0.5f;
    float halfHeight = rectangle.height() * 0.5f;
    if (intersection_R[0] > -halfWidth && intersection_R[0] < halfWidth &&
      intersection_R[1] > -halfHeight && intersection_R[1] < halfHeight) {
      return true;
    }
    return false;
  }
};

class AffordanceView : public views::View3D {
private:
  SceneModel& sceneModel;
  RectangleAffordances rectangleAffordances;
public:
  AffordanceView(SceneModel& model, int viewId) : views::View3D(viewId), sceneModel(model),
    rectangleAffordances(sceneModel) {

  }

  bool leftButtonDown(const ViewContext3D& viewContext) {
    return rectangleAffordances.leftButtonDown(viewContext);
  }

  bool mouseMoved(const ViewContext3D& viewContext) {
    return rectangleAffordances.mouseMoved(viewContext);
  }

  bool leftButtonUp(const ViewContext3D& viewContext) {
    return rectangleAffordances.leftButtonUp(viewContext);
  }

  void render(const ViewContext3D& context) const {
  }
};

}

