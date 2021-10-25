#include "views/add_rectangle_view.h"
#include <bgfx/bgfx.h>

namespace views {

const float PointRadius = 0.02;

static const uint16_t indices[] = { 0, 1, 2, 0, 2, 3 };
static const float vertices[20] = {
  PointRadius,  -PointRadius, 0.0,  1.0, -1.0,
  PointRadius,   PointRadius, 0.0,  1.0,  1.0,
  -PointRadius,  PointRadius, 0.0, -1.0,  1.0,
  -PointRadius, -PointRadius, 0.0, -1.0, -1.0
};

class PointView {
private:
  bgfx::VertexBufferHandle vertexBuffer;
  bgfx::IndexBufferHandle indexBuffer;
  bgfx::VertexLayout vertexLayout;
  bgfx::ProgramHandle program;
  const std::vector<Vector3f>& points;
public:
  PointView(const std::vector<Vector3f>& ps) : points(ps) {
    vertexLayout.begin()
      .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
      .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
      .end();

    auto verticesRef = bgfx::makeRef(&vertices[0], sizeof(float) * 20);
    vertexBuffer = bgfx::createVertexBuffer(verticesRef, vertexLayout);
    indexBuffer = bgfx::createIndexBuffer(bgfx::makeRef(indices, sizeof(indices)));
  }

  void render() const {
  }
};


using namespace geometry;

AddRectangleView::AddRectangleView(SceneModel& model, Timeline& timeline, int viewId) : views::View3D(viewId), sceneModel(model), timeline(timeline) {
}

bool AddRectangleView::leftButtonUp(const ViewContext3D& viewContext) {
  std::cout << "left button up" << std::endl;
  return false;
}

bool AddRectangleView::leftButtonDown(const ViewContext3D& context) {
  return false;
}

bool AddRectangleView::mouseMoved(const ViewContext3D& context) {
  const Vector3f& rayDirection = context.camera.computeRayWorld(context.width, context.height,
                                                                    context.mousePositionX, context.mousePositionY);
  pointingAt = sceneModel.traceRay(context.camera.getPosition(), rayDirection);
  return false;
}

void AddRectangleView::render(const ViewContext3D& context) const {

}
} // namespace views
