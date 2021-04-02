#ifndef H_CAMERA
#define H_CAMERA
#include <eigen3/Eigen/Dense>
#include <bx/math.h>
#include <bgfx/bgfx.h>

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
    // Returns rotation that rotates the default camera position vector {0, 0, -1}
    // to the current position.
    const Vector3f defaultPosition(0.0, 0.0, -1.0);
    return Quaternionf::FromTwoVectors(defaultPosition, position.normalized()).normalized();
  }

  void setOrientation(const Quaternionf& orn) {
    float distance = position.norm();
    auto newPosition = distance * (orn * -Vector3f::UnitZ());
    setPosition(newPosition);
  }
};

#endif
