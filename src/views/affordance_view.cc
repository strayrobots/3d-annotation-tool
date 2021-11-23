#include "views/affordance_view.h"
#include "id.h"

namespace views {

using TMatrix = Eigen::Transform<float, 3, Eigen::Affine>;

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

RectangleAffordances::RectangleAffordances(SceneModel& sceneModel, Timeline& tl) : scene(sceneModel), timeline(tl) {}

bool RectangleAffordances::leftButtonDown(const ViewContext3D& viewContext) {
  for (auto& rect : scene.getRectangles()) {
    auto hitType = hitTest(viewContext, rect);
    if (hitType > None) {
      auto point = intersectionLocal(viewContext, rect);

      if (viewContext.modifiers == ModAlt) {
        Rectangle newRect(rect);
        newRect.id = IdFactory::nextId();
        auto command = std::make_unique<commands::AddRectangleCommand>(newRect);
        timeline.pushCommand(std::move(command));

        dragging = {
          .oldRectangle = newRect,
          .newRectangle = newRect,
          .dragPoint_R = point,
          .copying = true,
          .dragType = hitType
        };
      } else {
        dragging = {
          .oldRectangle = rect,
          .newRectangle = rect,
          .dragPoint_R = point,
          .copying = false,
          .dragType = hitType
        };
      }
      return true;
    }
  }
  return false;
}

bool RectangleAffordances::mouseMoved(const ViewContext3D& viewContext) {
  if (isActive()) {
    Dragging& d = dragging.value();
    auto intersection_R = intersectionLocal(viewContext, d.oldRectangle);
    if (d.dragType == HitType::Rotate) {
      AngleAxisf rotation_R = AngleAxisf(Quaternionf::FromTwoVectors(intersection_R, d.dragPoint_R));
      if (viewContext.modifiers == ModShift) {
        // If shift key is held down, make the rotation relative to the amount dragged.

        rotation_R.angle() = ((intersection_R - d.dragPoint_R).norm() / d.oldRectangle.width()) * 0.2 * M_PI;
      }
      d.newRectangle.orientation = rotation_R * d.oldRectangle.orientation;
    } else if (d.dragType == HitType::Resize) {
      // Compute diff in upper right quadrant.
      auto sizeDiff = intersection_R.array().abs().matrix() - d.dragPoint_R.array().abs().matrix();
      d.newRectangle.size = d.oldRectangle.size + sizeDiff.head<2>();
      auto T_RW = computeT_RW(d.oldRectangle);
      auto diff_R = intersection_R - d.dragPoint_R;
      d.newRectangle.center = d.oldRectangle.center + T_RW.rotation().transpose() * diff_R * 0.5;
    } else {
      auto T_RW = computeT_RW(d.oldRectangle);
      Vector3f diff_R = intersection_R - d.dragPoint_R;
      d.newRectangle.center = d.oldRectangle.center + T_RW.rotation().transpose() * diff_R;
    }
    scene.updateRectangle(d.newRectangle);
    return true;
  } else {
    for (auto& rect : scene.getRectangles()) {
      auto hitType = hitTest(viewContext, rect);
      if (hitType > None) {
        rect.rotateControl = true;
      } else {
        rect.rotateControl = false;
      }
    }
  }
  return false;
}

bool RectangleAffordances::leftButtonUp(const ViewContext3D& viewContext) {
  if (isActive()) {
    auto& d = dragging.value();
    if (!d.copying) {
      auto command = std::make_unique<commands::EditRectangleCommand>(
          d.oldRectangle,
          d.newRectangle);
      timeline.pushCommand(std::move(command));
    }
    dragging.reset();
    return true;
  }
  return false;
}

bool RectangleAffordances::isActive() const { return dragging.has_value(); }

RectangleAffordances::HitType RectangleAffordances::hitTest(const ViewContext3D& viewContext, const Rectangle& rectangle) {
  TMatrix T_RW = computeT_RW(rectangle);

  Vector3f o_W = viewContext.camera.getPosition();
  Vector3f o_R = T_RW * o_W;
  Vector3f ray_W = viewContext.rayWorld();
  // In local coordinates.
  Vector3f ray_R = T_RW.rotation() * ray_W;
  float t = -o_R[2] / ray_R[2];
  Vector3f intersection_R = o_R + t * ray_R;
  float width = rectangle.width();
  float height = rectangle.height();
  float halfWidth = width * 0.5f;
  float halfHeight = height * 0.5f;


  if (intersection_R.norm() / std::min(width, height) < 0.125f) {
    return HitType::Rotate;
  } else if (intersection_R[0] > -halfWidth && intersection_R[0] < halfWidth &&
    intersection_R[1] > -halfHeight && intersection_R[1] < halfHeight) {

    Vector2f upperRightQuadrantPoint = Vector2f(intersection_R[0], intersection_R[1]).array().abs();
    auto canonicalCorner = Vector2f(halfWidth, halfHeight);
    if ((upperRightQuadrantPoint - canonicalCorner).norm() < 0.2 * std::min(height, width)) {
      return HitType::Resize;
    }
    return HitType::Move;
  }
  return HitType::None;
}

AffordanceView::AffordanceView(SceneModel& model, Timeline& timeline, int viewId) : views::View3D(viewId),
  sceneModel(model), timeline(timeline),
  rectangleAffordances(sceneModel, timeline) {

}

bool AffordanceView::leftButtonDown(const ViewContext3D& viewContext) {
  return rectangleAffordances.leftButtonDown(viewContext);
}

bool AffordanceView::mouseMoved(const ViewContext3D& viewContext) {
  return rectangleAffordances.mouseMoved(viewContext);
}

bool AffordanceView::leftButtonUp(const ViewContext3D& viewContext) {
  return rectangleAffordances.leftButtonUp(viewContext);
}

void AffordanceView::render(const ViewContext3D& context) const {
}

}

