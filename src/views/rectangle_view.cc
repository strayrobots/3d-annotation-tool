#include "views/rectangle_view.h"
#include "shader_utils.h"
#include <cassert>

namespace views {

static const uint16_t indices[] = { 0, 2, 1, 1, 3, 0 };
static const float textureCoordinates[4][2] = {
  {0.0f, 0.0f},
  {1.0f, 1.0f},
  {1.0f, 0.0f},
  {0.0f, 1.0f},
};

RectangleView::RectangleView(int id) : views::View3D(id), vertexData(4) {
  vertexLayout.begin()
    .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
    .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
    .end();

  vertexBuffer = bgfx::createDynamicVertexBuffer(4, vertexLayout);
  indexBuffer = bgfx::createIndexBuffer(bgfx::makeRef(indices, sizeof(indices)));
  u_rotation = bgfx::createUniform("u_rotation", bgfx::UniformType::Mat4);

  program = shader_utils::loadProgram("vs_rectangle", "fs_rectangle");
}

void RectangleView::clearVertices() {
  vertexData.clear();
}

void RectangleView::setVertices(const std::vector<Vector3f>& vs) {
  assert(vs.size() <= 4);
  for (unsigned int i=0; i < vs.size(); i++) {
    vertexData[i][0] = vs[i][0];
    vertexData[i][1] = vs[i][1];
    vertexData[i][2] = vs[i][2];
    vertexData[i][3] = textureCoordinates[i][0];
    vertexData[i][4] = textureCoordinates[i][1];
  }
  auto ref = bgfx::makeRef(&vertexData[0], sizeof(float) * 4 * 5);
  bgfx::update(vertexBuffer, 0, ref);
}

void RectangleView::render(const ViewContext3D& context) const {
  setCameraTransform(context);

  //Eigen::Matrix4f rotation = Matrix4f::Identity();
  //rotation.block<3, 3>(0, 0) = Eigen::Matrix3f(camera.getOrientation());
  //// u_rotation rotates the vertexData of the circle and keeps the disks facing
  //// towards the camera, regardless of the viewpoint.
  //bgfx::setUniform(u_rotation, rotation.data(), 1);

  bgfx::setVertexBuffer(0, vertexBuffer);
  bgfx::setIndexBuffer(indexBuffer);
  bgfx::setState(BGFX_STATE_DEFAULT | BGFX_STATE_BLEND_ALPHA);
  bgfx::submit(viewId, program);
}

}
