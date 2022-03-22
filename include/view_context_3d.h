#pragma once
#include "camera.h"
#include "input.h"
#include <optional>

class ViewContext3D {
public:
  Camera camera;

  ViewContext3D(const SceneCamera& cam) : camera(cam) {}
  ViewContext3D() : camera() {}
  double width = 800;
  double height = 600;
  double mousePositionX = 0.0;
  double mousePositionY = 0.0;

  // Modifier keys that are held down.
  InputModifier modifiers = 0;

  Vector3f rayWorld() const;
  std::optional<Vector3f> pointingAt;
  std::optional<Vector3f> pointingAtNormal;
};
