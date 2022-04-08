#include "views/affordance_view.h"
#include "id.h"

namespace views {

using TMatrix = Eigen::Transform<float, 3, Eigen::Affine>;

AffordanceView::AffordanceView(SceneModel& model, Timeline& timeline, int viewId) : views::View3D(viewId),
  sceneModel(model), timeline(timeline),
  rectangleAffordances(sceneModel, timeline) {

}

bool AffordanceView::leftButtonDown(const ViewContext3D& viewContext) {
  return rectangleAffordances.leftButtonDown(viewContext);
}

bool AffordanceView::mouseMoved(const ViewContext3D& viewContext) {
  return rectangleAffordances.mouseMoved(viewContext);
}

bool AffordanceView::leftButtonUp(const ViewContext3D& viewContext) {
  return rectangleAffordances.leftButtonUp(viewContext);
}

void AffordanceView::render(const ViewContext3D& context) const {
}

}

