#include "views/view.h"
#include <vector>
#include <bgfx/bgfx.h>

namespace views {
class RectangleView : public views::View3D {
private:
  std::vector<std::array<float, 5>> vertexData;

  bgfx::DynamicVertexBufferHandle vertexBuffer;
  bgfx::IndexBufferHandle indexBuffer;
  bgfx::VertexLayout vertexLayout;
  bgfx::ProgramHandle program;
  bgfx::UniformHandle u_rotation;
public:
  RectangleView(int id);
  void clearVertices();
  void setVertices(const std::vector<Vector3f>&);
  void render(const ViewContext3D& context) const;
};
}
