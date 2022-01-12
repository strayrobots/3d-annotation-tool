#pragma once
#include "eigen.h"
#include <bgfx/bgfx.h>
#include <vector>
#include <memory>
#include "views/view.h"
#include "geometry/mesh.h"

namespace views {

using namespace Eigen;

class MeshDrawable : views::View3D {
private:
  bgfx::UniformHandle u_color, u_lightDir;
  std::shared_ptr<geometry::TriangleMesh> mesh;
  Eigen::Matrix<float, Eigen::Dynamic, 7, Eigen::RowMajor> vertexData;

  // Uniform data.
  Vector4f lightDir;

  // Rendering.
  bgfx::VertexBufferHandle vertexBuffer;
  bgfx::IndexBufferHandle indexBuffer;
  bgfx::VertexLayout layout;
  bgfx::ProgramHandle uniformProgram;
  bgfx::ProgramHandle colorProgram;

public:
  MeshDrawable(std::shared_ptr<geometry::TriangleMesh> m, int viewId = 0);
  ~MeshDrawable();

  void setDrawingGeometry() const;
  void packVertexData();
  void createBuffers();
  void render(const ViewContext3D& context, const Matrix4f& T = Matrix4f::Identity(), const Vector4f& color = Vector4f(0.92, 0.59, 0.2, 1.0)) const;
};

} // namespace views
