#ifndef H_CAMERA
#define H_CAMERA
#include <eigen3/Eigen/Dense>

using namespace Eigen;

class Camera {
private:
  Vector3f position;
  Vector3f lookat;
  Quaternionf orientation;
  Affine3f viewMatrix;

  void setViewMatrix(const Affine3f& newViewMatrix) { viewMatrix = newViewMatrix;}
  void setPosition(const Vector3f& p) { position = p;}
  void setLookat(const Vector3f& l) { lookat = l; }
  void setOrientation(const Quaternionf& q) { orientation = q;}

  Matrix3f getCameraRotation(const Vector3f& forwardVector) const;

public:
  const float fov = 30.0f;
  Camera(Vector3f initialLookat, float distance);

  const Vector3f& getPosition() const { return position; }

  const Vector3f& getLookat() const { return lookat; }

  const Quaternionf& getOrientation() const { return orientation; }

  const Affine3f& getViewMatrix() const { return viewMatrix; }

  Vector3f getForwardVector(void) const;
  Vector3f getUpVector(void) const;
  Vector3f getRightVector(void) const;
  Vector3f computeRayWorld(float width, float height, double x, double y) const;

  void updatePosition(const Vector3f& p);
  void updateLookat(const Vector3f& newLookat);
  void rotateAroundTarget(const Quaternionf& q);
  void zoom(float d);
};

#endif
