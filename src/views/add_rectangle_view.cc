#include <iostream>
#include "views/add_rectangle_view.h"
#include "commands/rectangle.h"

namespace views {

using namespace geometry;
using namespace commands;

AddRectangleView::AddRectangleView(SceneModel& model, Timeline& timeline, int viewId) : views::View3D(viewId), sceneModel(model), timeline(timeline),
  pointView(viewId), rectangleView(viewId) {
}

bool AddRectangleView::leftButtonUp(const ViewContext3D& viewContext) {
  if (pointingAt.has_value()) {
    if (rectangleCorners.size() == 3) {
      rectangleCorners.clear();
      pointView.clearPoints();
    }
    rectangleCorners.push_back(pointingAt.value());
    pointView.addPoint(pointingAt.value());
    computeVertices();
    if (rectangleCorners.size() == 3) {
      commit();
    }
    return true;
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

  if (rectangleCorners.size() == 2) {
    computeVertices();
  }
  return false;
}

void AddRectangleView::computeVertices() {
  if (!hasRectangle()) return;
  Vector3f v1 = rectangleCorners[0];
  Vector3f v2 = rectangleCorners[1];
  Vector3f v3;
  if (rectangleCorners.size() == 3) {
    v3 = rectangleCorners[2];
  } else if (pointingAt.has_value()) {
    v3 = pointingAt.value();
  } else {
    return;
  }
  if ((v3 - v2).norm() < (v3 - v1).norm()) {
    // v1 is taken to be the point further away from the third point
    // v2 is taken to be the point closer to the third point.
    v1 = rectangleCorners[1];
    v2 = rectangleCorners[0];
  }

  // Vector from v1 to v2.
  Vector3f firstEdge = v2 - v1;
  Vector3f nextEdge = v3 - v2;
  float edgeNorm = firstEdge.norm();
  // Project vector going from v2 to v3 onto plane defined by the edge going from
  // v1 to v2.
  nextEdge = nextEdge - nextEdge.dot(firstEdge) / (edgeNorm*edgeNorm) * firstEdge;
  v3 = v2 + nextEdge;
  Vector3f v4 = v1 + nextEdge;

  vertices[0] = v1;
  vertices[1] = v2;
  vertices[2] = v4;
  vertices[3] = v3;
}

void AddRectangleView::commit() {
  auto command = std::make_unique<AddRectangleCommand>(vertices, sceneModel.currentInstanceId);
  timeline.pushCommand(std::move(command));
  rectangleCorners.clear();
  pointView.clearPoints();
}

bool AddRectangleView::hasRectangle() const {
  return rectangleCorners.size() >= 3 || (rectangleCorners.size() >= 2 && pointingAt.has_value());
}

void AddRectangleView::render(const ViewContext3D& context) const {
  pointView.render(context);
  if (hasRectangle()) {
    Rectangle rectangle(vertices);
    rectangleView.render(context, rectangle);
  }
}
} // namespace views
