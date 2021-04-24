#ifndef H_MESH_VIEW
#define H_MESH_VIEW
#include <eigen3/Eigen/Dense>
#include <bgfx/bgfx.h>
#include <vector>
#include <memory>
#include "views/view.h"
#include "geometry/mesh.h"

namespace views
{

using namespace Eigen;

class MeshDrawable
{
private:
  std::shared_ptr<geometry::TriangleMesh> mesh;
  Eigen::Matrix<float, Eigen::Dynamic, 7, Eigen::RowMajor> vertexData;

  // Uniform data.
  Vector4f defaultColor;

  // Rendering.
  bgfx::VertexBufferHandle vertexBuffer;
  bgfx::IndexBufferHandle indexBuffer;
  bgfx::VertexLayout layout;

public:
  MeshDrawable(std::shared_ptr<geometry::TriangleMesh> m, const Vector4f& c = Vector4f(0.92, 0.59, 0.2, 1.0));
  ~MeshDrawable();

  void setDrawingGeometry() const;
  void packVertexData();
  void createBuffers();
};

class MeshView : public views::View
{
private:
  bgfx::UniformHandle u_color, u_lightDir;
  bgfx::ProgramHandle program;
  std::vector<std::shared_ptr<views::MeshDrawable>> objects;
  Vector4f lightDir;

public:
  MeshView(int width, int height);
  ~MeshView();
  const std::vector<std::shared_ptr<views::MeshDrawable>>& getObjects() const { return objects; };

  void addObject(std::shared_ptr<MeshDrawable> obj);
  void popObject() { objects.pop_back(); }

  virtual void render(const Camera& camera) const override;
};

} // namespace views
#endif
