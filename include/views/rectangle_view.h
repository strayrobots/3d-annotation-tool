#pragma once
#include "views/view.h"
#include "scene_model.h"
#include <vector>
#include <bgfx/bgfx.h>

namespace views {
class RectangleView {
private:
  int viewId;

  bgfx::VertexBufferHandle vertexBuffer;
  bgfx::IndexBufferHandle indexBuffer;
  bgfx::VertexLayout vertexLayout;
  bgfx::ProgramHandle program;
  bgfx::UniformHandle u_scale;
public:
  RectangleView(int id);
  ~RectangleView();
  void setVertices(const std::array<Vector3f, 4>&);
  void render(const Rectangle& rectangle) const;
};
}
