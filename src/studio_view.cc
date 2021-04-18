#include "studio_view.h"

StudioView::StudioView(const SceneModel& model) : sceneModel(model) {
  meshView = std::make_shared<views::MeshView>();
  meshDrawable = std::make_shared<views::MeshDrawable>(sceneModel.getMesh());
  meshView->addObject(meshDrawable);
}

void StudioView::render(std::shared_ptr<Camera> camera) const {
  meshView->render(camera);
}

