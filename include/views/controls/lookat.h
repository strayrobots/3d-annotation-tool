#pragma once
#include <optional>
#include <functional>
#include <memory>
#include <bgfx/bgfx.h>
#include <eigen3/Eigen/Geometry>
#include "geometry/mesh.h"
#include "geometry/ray_trace_mesh.h"
#include "views/mesh_view.h"
#include "views/controls/control.h"
#include "view_context_3d.h"

namespace views::controls {

class LookatControl : public views::controls::Control {
private:
  const Vector4f lightDir = Vector4f(0.0, 1.0, -1.0, 1.0);
  views::MeshDrawable sphereDrawable;
  // views::MeshDrawable sphereDrawable;
  //  Rendering.
  bgfx::ProgramHandle program;
  bgfx::UniformHandle u_color, u_lightDir;
  int activeAxis = -1;

public:
  LookatControl(int viewId);
  ~LookatControl(){};
  void render(const ViewContext3D& context) const;
};

} // namespace views::controls
