#include "views/bbox.h"
#include "shader_utils.h"

namespace views {
static float cubeVertices[] = {
   0.5f, -0.5f, -0.5f,
   0.5f,  0.5f, -0.5f,
  -0.5f,  0.5f, -0.5f,
  -0.5f, -0.5f, -0.5f,
   0.5f, -0.5f,  0.5f,
   0.5f,  0.5f,  0.5f,
  -0.5f, -0.5f,  0.5f,
  -0.5f,  0.5f,  0.5f
};

static const uint16_t cubeTriangleList[] = {
  0, 1,
  0, 3,
  0, 4,
  2, 1,
  2, 3,
  2, 7,
  6, 3,
  6, 4,
  6, 7,
  5, 1,
  5, 4,
  5, 7,
};

BBoxView::BBoxView(int viewId) : View3D(viewId) {
  layout.begin()
      .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
      .end();

  vertexBuffer = bgfx::createVertexBuffer(bgfx::makeRef(&cubeVertices[0], 8 * 3 * sizeof(float)), layout);
  indexBuffer = bgfx::createIndexBuffer(bgfx::makeRef(&cubeTriangleList[0], 24 * sizeof(uint16_t)));
  u_scale = bgfx::createUniform("u_scale", bgfx::UniformType::Vec4);
  program = shader_utils::loadProgram("vs_bbox", "fs_bbox");
}

BBoxView::~BBoxView() {
  bgfx::destroy(indexBuffer);
  bgfx::destroy(vertexBuffer);
  bgfx::destroy(program);
  bgfx::destroy(u_scale);
}

void BBoxView::render(const BBox& bbox) const {
  Matrix4f T = Matrix4f::Identity();
  T.block<3, 3>(0, 0) = bbox.orientation.toRotationMatrix();
  T.block<3, 1>(0, 3) = bbox.position;
  bgfx::setTransform(T.data());
  bgfx::setIndexBuffer(indexBuffer);
  bgfx::setVertexBuffer(0, vertexBuffer);
  Vector4f scale(bbox.dimensions[0], bbox.dimensions[1], bbox.dimensions[2], 1.0);
  bgfx::setUniform(u_scale, scale.data(), 1);
  bgfx::setState(BGFX_STATE_DEFAULT |
      BGFX_STATE_PT_LINES |
      BGFX_STATE_WRITE_A |
      BGFX_STATE_WRITE_RGB |
      BGFX_STATE_WRITE_Z |
      BGFX_STATE_CULL_CW |
      BGFX_STATE_BLEND_ALPHA |
      BGFX_STATE_MSAA);
  bgfx::submit(viewId, program);
}

}

