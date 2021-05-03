#include "views/view.h"
#include "camera.h"
#include <bgfx/bgfx.h>
#include <bx/math.h>

namespace views {
void setCameraTransform(const ViewContext3D& context) {
  const auto camera = context.camera;
  float proj[16];
  float view[16];

  auto position = camera.getPosition();
  auto lookat = camera.getLookat();
  auto cameraUp = camera.getUpVector();

  const bx::Vec3 at = {lookat[0], lookat[1], lookat[2]};
  const bx::Vec3 eye = {position[0], position[1], position[2]};
  const bx::Vec3 up = {cameraUp[0], cameraUp[1], cameraUp[2]};
  bx::mtxProj(proj, camera.fov, float(context.width) / float(context.height), 0.1f, 25.0f, bgfx::getCaps()->homogeneousDepth, bx::Handness::Right);
  bx::mtxLookAt(view, eye, at, up, bx::Handness::Right);

  bgfx::setViewTransform(0, view, proj);
}
}
