#include "views/view.h"
#include "camera.h"
#include <bgfx/bgfx.h>
#include <bx/math.h>

namespace views {
void View::setCameraTransform(const ViewContext3D& context) const {
  const auto camera = context.camera;
  float proj[16];
  bx::mtxProj(proj, camera.fov, float(context.width) / float(context.height), 0.1f, 25.0f, bgfx::getCaps()->homogeneousDepth, bx::Handness::Right);
  bgfx::setViewTransform(viewId, camera.getViewMatrix().data(), proj);
}
}
