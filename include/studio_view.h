#ifndef H_STUDIO_VIEW
#define H_STUDIO_VIEW
#include "views/mesh_view.h"
#include "views/view.h"
#include "scene_model.h"

class StudioView : public views::View {
private:
  const SceneModel& sceneModel;
  std::shared_ptr<views::MeshDrawable> meshDrawable;
public:
  std::shared_ptr<views::MeshView> meshView;

  StudioView(const SceneModel& model);
  void render(const Camera& camera) const override;
};

#endif
