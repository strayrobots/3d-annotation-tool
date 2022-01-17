#include <bgfx/bgfx.h>
#include "scene_model.h"
#include "views/view.h"

namespace views {
class PointCloudView : public views::View3D {
private:
  struct VertexData {
    float x;
    float y;
    float z;
    uint32_t rgba;
  };

  SceneModel& scene;
  std::vector<VertexData> vertexData;
  std::vector<uint32_t> indices;
  bgfx::VertexBufferHandle vertexBuffer;
  bgfx::IndexBufferHandle indexBuffer;
  bgfx::VertexLayout layout;
  bgfx::ProgramHandle program;
  bool initialized = false;
public:
  PointCloudView(SceneModel& model, int viewId);
  ~PointCloudView();
  void loadPointCloud();
  void packVertexData();
  void render(const ViewContext3D& context) const;
};
}
