#include "views/view.h"
#include <vector>
#include <bgfx/bgfx.h>

namespace views {
class PointView : public views::View3D {
private:
  std::vector<Vector3f> points;

  bgfx::VertexBufferHandle vertexBuffer;
  bgfx::IndexBufferHandle indexBuffer;
  bgfx::VertexLayout vertexLayout;
  bgfx::ProgramHandle program;
  bgfx::UniformHandle u_rotation;
public:
  PointView(int id);
  ~PointView();
  void clearPoints();
  void addPoint(const Vector3f& point);
  void render(const ViewContext3D& context) const;
};
}
