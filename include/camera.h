#pragma once
#include <eigen3/Eigen/Dense>

using namespace Eigen;

class Camera {
private:
  Vector3f position;
public:
  const float fov = 30.0f;
  Camera(const Vector3f& p) : position(p) {}

  const Vector3f& getPosition() const {
    return position;
  }

  void setPosition(const Vector3f p) {
    position = p;
  }

  Vector3f getUpVector() const {
    return getOrientation() * Vector3f::UnitY();
  }

  Quaternionf getOrientation() const {
    // Assumes that the camera points to the origin.
    // Returns rotation that rotates the default camera position vector
    // to the current position.
    const Vector3f defaultPosition(0.0, 0.0, -1.0);
    return Quaternionf::FromTwoVectors(defaultPosition, position.normalized()).normalized();
  }

  void setOrientation(const Quaternionf& orn) {
    float distance = position.norm();
    auto newPosition = distance * (orn * -Vector3f::UnitZ());
    setPosition(newPosition);
  }

  Vector3f computeRay(double width, double height, double x, double y) {
    double aspectRatio = width / height;
    double pX = (2.0 * (x / width) - 1.0) * std::tan(fov / 2.0 * M_PI / 180) * aspectRatio;
    double pY = (1.0 - 2.0 * (y / height)) * std::tan(fov / 2.0 * M_PI / 180);
    Vector3f cameraRay(pX, pY, 1.0);
    return getOrientation() * cameraRay.normalized();
  }
};

