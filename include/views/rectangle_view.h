#pragma once
#include <vector>
#include <bgfx/bgfx.h>
#include "views/view.h"
#include "view_context_3d.h"
#include "scene_model.h"

namespace views {
class RectangleView : views::View3D{
private:
  bgfx::VertexBufferHandle vertexBuffer;
  bgfx::IndexBufferHandle indexBuffer;
  bgfx::VertexLayout vertexLayout;
  bgfx::ProgramHandle program;
  bgfx::UniformHandle u_scale;
public:
  RectangleView(int id);
  ~RectangleView();
  void setVertices(const std::array<Vector3f, 4>&);
  void render(const ViewContext3D& context, const Rectangle& rectangle) const;
};
}
