#pragma once
#include <eigen3/Eigen/Dense>

using namespace Eigen;

struct Rectangle {
  int id;
  int classId;
  Vector3f center;
  // Rotation from canonical world coordinates to local coordinates.
  Quaternionf orientation;
  Vector2f size; // width, height.
  bool rotateControl = false;
  Rectangle();
  Rectangle(const Rectangle& rectangle);
  Rectangle(const std::array<Vector3f, 4>&);
  Rectangle(int id, int classId, Vector3f center, Quaternionf orientation, Vector2f size);
  float width() const;
  float height() const;
  Vector3f normal() const;
};


