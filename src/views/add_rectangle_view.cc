#include "views/add_rectangle_view.h"

namespace views {

using namespace geometry;

AddRectangleView::AddRectangleView(SceneModel& model, Timeline& timeline, int viewId) : views::View3D(viewId), sceneModel(model), timeline(timeline),
  pointView(viewId) {
}

bool AddRectangleView::leftButtonUp(const ViewContext3D& viewContext) {
  std::cout << "left button up" << std::endl;
  if (pointingAt.has_value()) {
    pointView.addPoint(pointingAt.value());
  }
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
  pointView.render(context);

}
} // namespace views
