#include "view_context_3d.h"

Vector3f ViewContext3D::rayWorld() const {
  return camera.computeRayWorld(width, height, mousePositionX, mousePositionY);
};
