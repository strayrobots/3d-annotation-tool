#include "views/rectangle_view.h"
#include "shader_utils.h"
#include <cassert>

namespace views {

static const uint16_t indices[] = {
  0, 3, 1,
  0, 1, 3,
  0, 3, 2,
  0, 2, 3,
}; // Render triangles both front and back.
static const float vertexData[] = {
  -1.0f, -1.0f, 0.0f,
  -1.0f,  1.0f, 0.0f,
   1.0f, -1.0f, 0.0f,
   1.0f,  1.0f, 0.0f,
};

RectangleView::RectangleView(int id) : viewId(id) {
  vertexLayout.begin()
    .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
    .end();

  vertexBuffer = bgfx::createVertexBuffer(bgfx::makeRef(&vertexData[0], 4 * 3 * sizeof(float)), vertexLayout);
  indexBuffer = bgfx::createIndexBuffer(bgfx::makeRef(indices, 4 * 3 * sizeof(uint16_t)));

  u_scale = bgfx::createUniform("u_scale", bgfx::UniformType::Vec4);
  program = shader_utils::loadProgram("vs_rectangle", "fs_rectangle");
}

RectangleView::~RectangleView() {
  bgfx::destroy(indexBuffer);
  bgfx::destroy(vertexBuffer);
  bgfx::destroy(program);
  bgfx::destroy(u_scale);
}

void RectangleView::render(const Rectangle& rectangle) const {
  auto center = rectangle.center;
  Eigen::Matrix4f transform = Matrix4f::Identity();
  Eigen::Matrix3f rotation(rectangle.orientation);
  transform.block<3, 3>(0, 0) = rotation;
  transform.block<3, 1>(0, 3) = center;

  // Canonical rectangle vertices are actually 2m x 2m.
  float ratioX = rectangle.width() * 0.5f;
  float ratioY = rectangle.height() * 0.5f;
  Vector4f scale(ratioX, ratioY, 0.0f, -1.0f);
  if (rectangle.rotateControl) {
    scale[3] = 1.0f;
  }
  bgfx::setUniform(u_scale, scale.data(), 1);

  bgfx::setTransform(transform.data());
  bgfx::setVertexBuffer(0, vertexBuffer);
  bgfx::setIndexBuffer(indexBuffer);
  bgfx::setState(BGFX_STATE_DEFAULT
      | BGFX_STATE_WRITE_RGB
      | BGFX_STATE_WRITE_A
      | BGFX_STATE_WRITE_Z
      | BGFX_STATE_BLEND_ALPHA
      | BGFX_STATE_MSAA);
  bgfx::submit(viewId, program);
}

}
