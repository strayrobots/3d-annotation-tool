#pragma once
#include <optional>
#include "commands/rectangle.h"
#include "views/view.h"
#include "scene_model.h"

namespace views {

using TMatrix = Eigen::Transform<float, 3, Eigen::Affine>;

class RectangleAffordances : public views::View3D {
private:
  struct Dragging {
    Rectangle oldRectangle;
    Rectangle newRectangle;
    Vector3f dragPoint_R; // drag point in local coordinates.
  };
  SceneModel& scene;
  Timeline& timeline;
  std::optional<Dragging> dragging;
public:
  RectangleAffordances(SceneModel& sceneModel, Timeline& tl) : scene(sceneModel), timeline(tl) {}
  bool leftButtonDown(const ViewContext3D& viewContext) {
    for (auto& rect : scene.getRectangles()) {
      if (hitTest(viewContext, rect)) {
        auto point = intersectionLocal(viewContext, rect);
        dragging = {
          .oldRectangle = rect,
          .newRectangle = rect,
          .dragPoint_R = point,
        };
        return true;
      }
    }
    return false;
  }

  bool mouseMoved(const ViewContext3D& viewContext) {
    if (isActive()) {
      Dragging& d = dragging.value();
      auto intersection_R = intersectionLocal(viewContext, d.oldRectangle);
      auto T_RW = computeT_RW(d.oldRectangle);
      Vector3f diff_R = intersection_R - d.dragPoint_R;
      d.newRectangle.center = d.oldRectangle.center + T_RW.rotation().transpose() * diff_R;
      scene.updateRectangle(d.newRectangle);
      return true;
    }
    return false;
  }

  bool leftButtonUp(const ViewContext3D& viewContext) {
    if (isActive()) {
      auto& d = dragging.value();
      auto command = std::make_unique<commands::MoveRectangleCommand>(
          d.newRectangle,
          d.oldRectangle.center,
          d.newRectangle.center);
      timeline.pushCommand(std::move(command));
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
  Timeline& timeline;
  RectangleAffordances rectangleAffordances;
public:
  AffordanceView(SceneModel& model, Timeline& timeline, int viewId) : views::View3D(viewId),
    sceneModel(model), timeline(timeline),
    rectangleAffordances(sceneModel, timeline) {

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

