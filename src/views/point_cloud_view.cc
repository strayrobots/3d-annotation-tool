#include "views/point_cloud_view.h"
#include "geometry/point_cloud.h"
#include "shader_utils.h"

namespace views {
PointCloudView::PointCloudView(SceneModel& model, int viewId) : views::View3D(viewId), scene(model) {
  layout.begin()
    .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
    .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
    .end();
};

PointCloudView::~PointCloudView() {
  bgfx::destroy(vertexBuffer);
  bgfx::destroy(indexBuffer);
  bgfx::destroy(program);
}

void PointCloudView::packVertexData() {
  auto pointCloud = scene.getPointCloud();
  auto& V = pointCloud->points;
  auto& C = pointCloud->colors;
  vertexData.resize(V.rows());
  for (unsigned int i=0; i < V.rows(); i++) {
    vertexData[i].x = V(i, 0);
    vertexData[i].y = V(i, 1);
    vertexData[i].z = V(i, 2);
    vertexData[i].rgba = (0xff << 24) + (C(i, 2) << 16) + (C(i, 1) << 8) + C(i, 0);
  }
}

void PointCloudView::loadPointCloud() {
  if (initialized) return;
  packVertexData();
  vertexBuffer = bgfx::createVertexBuffer(bgfx::makeRef(vertexData.data(), vertexData.size() * sizeof(VertexData)), layout);
  indices.resize(vertexData.size());
  // Could not figure out a way to draw unindexed in bgfx.
  for (uint32_t i=0; i < vertexData.size(); i++) {
    indices[i] = i;
  }
  indexBuffer = bgfx::createIndexBuffer(bgfx::makeRef(indices.data(), indices.size() * sizeof(uint32_t)), BGFX_BUFFER_INDEX32);

  program = shader_utils::loadProgram("vs_point_cloud", "fs_point_cloud");
  initialized = true;
}

void PointCloudView::changeSize(float diff) {
  scene.pointCloudPointSize = std::clamp(scene.pointCloudPointSize + diff, 1.0f, 10.0f);
}

void PointCloudView::render(const ViewContext3D& context) const {
  if (!initialized) return;
  setCameraTransform(context);
  unsigned int size = std::round(scene.pointCloudPointSize);
  bgfx::setState(BGFX_STATE_DEFAULT | BGFX_STATE_PT_POINTS
      | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A
      | BGFX_STATE_BLEND_ALPHA
      | BGFX_STATE_POINT_SIZE(size));
  bgfx::setVertexBuffer(0, vertexBuffer);
  bgfx::setIndexBuffer(indexBuffer);
  bgfx::submit(viewId, program);
}
}
