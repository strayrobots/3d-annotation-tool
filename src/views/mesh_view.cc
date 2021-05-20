#include <omp.h>
#include <3rdparty/happly.h>
#include <eigen3/Eigen/Dense>
#include <bx/math.h>
#include "views/view.h"
#include "shader_utils.h"
#include "views/mesh_view.h"

namespace views {

using namespace Eigen;

using RowMatrixf = Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor>;
using RowMatrixi = Eigen::Matrix<uint32_t, Eigen::Dynamic, 3, Eigen::RowMajor>;
using TriangleFace = Eigen::Matrix<uint32_t, 1, 3, Eigen::RowMajor>;

MeshDrawable::MeshDrawable(std::shared_ptr<geometry::TriangleMesh> m, int viewId) : views::View3D(viewId), mesh(m), lightDir(0.0, 1.0, -1.0, 1.0) {
  layout.begin()
      .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
      .add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float, true)
      .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
      .end();
  packVertexData();
  createBuffers();

  u_lightDir = bgfx::createUniform("u_light_dir", bgfx::UniformType::Vec4);
  u_color = bgfx::createUniform("u_color", bgfx::UniformType::Vec4);
  uniformProgram = shader_utils::loadProgram("vs_mesh", "fs_mesh_uniform");
  colorProgram = shader_utils::loadProgram("vs_mesh", "fs_mesh_colors");
}

MeshDrawable::~MeshDrawable() {
  bgfx::destroy(indexBuffer);
  bgfx::destroy(vertexBuffer);
  bgfx::destroy(uniformProgram);
  bgfx::destroy(colorProgram);
  bgfx::destroy(u_lightDir);
  bgfx::destroy(u_color);
}

void MeshDrawable::createBuffers() {
  const auto& F = mesh->faces();
  indexBuffer = bgfx::createIndexBuffer(bgfx::makeRef(F.data(), F.rows() * F.cols() * sizeof(uint32_t)), BGFX_BUFFER_INDEX32);
  vertexBuffer = bgfx::createVertexBuffer(bgfx::makeRef(vertexData.data(), vertexData.rows() * vertexData.cols() * sizeof(float)), layout);
}

void MeshDrawable::packVertexData() {
  const auto& V = mesh->vertices();
  const auto& N = mesh->getVertexNormals();

  const auto& vertexColors = mesh->getVertexColors();
  const bool colorsFromFile = mesh->colorsFromFile;
  vertexData.resize(V.rows(), 7);

#pragma omp parallel for
  for (int i = 0; i < V.rows(); i++) {
    vertexData.block<1, 3>(i, 0) = V.row(i);
    vertexData.block<1, 3>(i, 3) = N.row(i);

    if (colorsFromFile) {
      uint32_t vertexColor = (0xff << 24) + (vertexColors(i, 2) << 16) + (vertexColors(i, 1) << 8) + vertexColors(i, 0);
      std::memcpy(&vertexData(i, 6), &vertexColor, sizeof(vertexColor));
    }
  }
}

void MeshDrawable::setDrawingGeometry() const {
  bgfx::setVertexBuffer(0, vertexBuffer);
  bgfx::setIndexBuffer(indexBuffer);
}

void MeshDrawable::render(const ViewContext3D& context, const Matrix4f& T, const Vector4f& color) const {
  setCameraTransform(context);
  bgfx::setUniform(u_lightDir, lightDir.data(), 1);
  bgfx::setUniform(u_color, color.data(), 1);
  bgfx::setTransform(T.data());
  setDrawingGeometry();
  bgfx::setState(BGFX_STATE_DEFAULT | BGFX_STATE_CULL_CW | BGFX_STATE_MSAA | BGFX_STATE_WRITE_A | BGFX_STATE_WRITE_RGB | BGFX_STATE_BLEND_ALPHA);
  if (mesh->colorsFromFile) {
    bgfx::submit(viewId, colorProgram);
  } else {
    bgfx::submit(viewId, uniformProgram);
  }
}

} // namespace views
