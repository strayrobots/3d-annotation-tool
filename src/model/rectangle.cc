#include <array>
#include <algorithm>
#include "model/rectangle.h"

Rectangle::Rectangle(const Rectangle& rect) {
  id = rect.id;
  classId = rect.classId;
  center = rect.center;
  orientation = rect.orientation;
  size = rect.size;
}

Rectangle::Rectangle(const std::array<Vector3f, 4>& vertices) {
  std::array<Vector3f, 4> copy(vertices);
  Vector3f topLeft = vertices[0];
  std::vector<float> distances;
  std::transform(vertices.begin() + 1, vertices.end(), std::back_inserter(distances), [&](const Vector3f& v) -> float {
    return -(v - topLeft).norm();
  });
  int bottomRightIndex = std::min_element(distances.begin(), distances.end()) - distances.begin() + 1;
  // Move bottom right to last position.
  std::swap(copy[bottomRightIndex], copy[3]);

  Vector3f edge1 = (copy[2] - copy[0]);
  Vector3f edge2 = (copy[1] - copy[0]);
  Vector3f bottomRight = copy[3];

  center = (topLeft + bottomRight) / 2.0f;
  Vector3f normal = edge1.cross(edge2);
  normal = normal / normal.norm();

  Eigen::Matrix3f R_RW;
  R_RW.row(0) = edge1 / edge1.norm();
  R_RW.row(1) = edge2 / edge2.norm();
  R_RW.row(2) = normal;
  orientation = Quaternionf(R_RW.transpose());

  id = 0;
  classId = 0;
  size = Vector2f(edge1.norm(), edge2.norm());
}

Rectangle::Rectangle(int id, int classId, Vector3f center, Quaternionf orientation, Vector2f size) :
  id(id), classId(classId), center(center), orientation(orientation), size(size) {}

float Rectangle::width() const { return size[0]; }
float Rectangle::height() const { return size[1]; }

Vector3f Rectangle::normal() const {
  return orientation * Vector3f::UnitZ();
}

