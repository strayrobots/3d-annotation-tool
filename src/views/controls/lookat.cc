#include <bx/math.h>
#include "views/controls/lookat.h"
#include "views/view.h"
#include "geometry/mesh.h"
#include "views/mesh_view.h"
#include "shader_utils.h"
#include "views/controls/control.h"
#include "shader_utils.h"
#include "geometry/mesh.h"
#include "asset_utils.h"
#include <iostream>

namespace views::controls {

LookatControl::LookatControl(int viewId) : views::controls::Control(viewId), sphereDrawable(std::make_shared<geometry::Sphere>(Matrix4f::Identity(), 0.05), viewId) {
}

void LookatControl::render(const ViewContext3D& context) const {
  Matrix4f T = Matrix4f::Identity();
  T.block<3, 1>(0, 3) = context.camera.getLookat();
  const Vector4f color(1.0f, 0.0f, 0.0f, 1.0f);
  sphereDrawable.render(context, T, color);
}

} // namespace views::controls