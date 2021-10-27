#include <iostream>
#include "views/add_rectangle_view.h"

namespace views {

using namespace geometry;
const float Deg45 = 0.7853981633974483; // In radians.

AddRectangleView::AddRectangleView(SceneModel& model, Timeline& timeline, int viewId) : views::View3D(viewId), sceneModel(model), timeline(timeline),
  pointView(viewId), rectangleView(viewId) {
}

bool AddRectangleView::leftButtonUp(const ViewContext3D& viewContext) {
  if (rectangleCorners.size() == 2) {
    rectangleCorners.clear();
    pointView.clearPoints();
  }
  if (pointingAt.has_value()) {
    rectangleCorners.push_back(pointingAt.value());
    pointView.addPoint(pointingAt.value());
  }
  return false;
}

bool AddRectangleView::leftButtonDown(const ViewContext3D& context) {
  return false;
}

bool AddRectangleView::mouseMoved(const ViewContext3D& context) {
  const Vector3f& rayDirection = context.camera.computeRayWorld(context.width, context.height,
                                                                    context.mousePositionX, context.mousePositionY);
  auto intersection = sceneModel.traceRayIntersection(context.camera.getPosition(), rayDirection);
  if (intersection.hit) {
    pointingAt = intersection.point;
    pointingAtNormal = intersection.normal;
  }

  if (rectangleCorners.size() == 1) {
    drawRectangle();
  }
  return false;
}

void AddRectangleView::drawRectangle() {
  Vector3f start = rectangleCorners[0];
  Vector3f end = pointingAt.value();
  Vector3f normal = pointingAtNormal.value();
  Vector3f diagonal = start - end;

  // Project normal onto plane defined by diagonal vector.
  // This is the "fixed" normal.
  Vector3f n = normal - diagonal.dot(normal) / normal.norm() * normal;
  n = n / n.norm();
  auto tangent = diagonal.cross(n);
  float sideLength = std::cos(Deg45) * diagonal.norm();

  float diagonalNorm = diagonal.norm();
  float s2 = sideLength * sideLength;
  float d2 = diagonalNorm * diagonalNorm;
  Vector3f sideVector = (s2*diagonal)/(d2) + (
    (sideLength * std::sqrt(d2 - s2)) / diagonalNorm * tangent / tangent.norm()
    );

  Vector3f thirdVertex = end + sideVector;
  std::vector<Vector3f> vertices(4);
  vertices[0] = start - 0.005 * n;
  vertices[1] = end - 0.005 * n;
  vertices[2] = thirdVertex - 0.005 * n;
  vertices[3] = start - sideVector - 0.005 * n;
  rectangleView.setVertices(vertices);
}

void AddRectangleView::render(const ViewContext3D& context) const {
  pointView.render(context);
  rectangleView.render(context);
}
} // namespace views
