#include <iostream>
#include "views/add_rectangle_view.h"

namespace views {

using namespace geometry;

AddRectangleView::AddRectangleView(SceneModel& model, Timeline& timeline, int viewId) : views::View3D(viewId), sceneModel(model), timeline(timeline),
  pointView(viewId), rectangleView(viewId) {
}

bool AddRectangleView::leftButtonUp(const ViewContext3D& viewContext) {
  if (rectangleCorners.size() == 3) {
    rectangleCorners.clear();
    pointView.clearPoints();
  }
  if (pointingAt.has_value()) {
    rectangleCorners.push_back(pointingAt.value());
    normals.push_back(pointingAtNormal.value());
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

  if (rectangleCorners.size() == 2) {
    drawRectangle();
  }
  return false;
}

void AddRectangleView::drawRectangle() {
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

  Vector3f normal = Vector3f::Zero();
  for (int i=0; i < normals.size(); i++) {
    normal += normals[i];
  }
  Vector3f n = normal / normal.norm();
  std::vector<Vector3f> vertices(4);
  // Add slight clearance so the rectangle doesn't get sucked into the mesh.
  // TODO: Some clever shading could prioritize the rectangle so it would always be visible.
  vertices[0] = v1 - 0.005 * n;
  vertices[1] = v2 - 0.005 * n;
  vertices[2] = v3 - 0.005 * n;
  vertices[3] = v4 - 0.005 * n;
  rectangleView.setVertices(vertices);
}

void AddRectangleView::render(const ViewContext3D& context) const {
  pointView.render(context);
  rectangleView.render(context);
}
} // namespace views
