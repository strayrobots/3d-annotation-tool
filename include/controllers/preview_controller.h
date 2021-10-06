#include "controllers/controller.h"
#include "views/image_pane.h"
#include "scene_model.h"

namespace controllers {
class PreviewController : public controllers::Controller {
private:
  int viewId;
  std::unique_ptr<views::ImagePane> imageView;
  const SceneModel& model;
public:
  PreviewController(const SceneModel& model, int viewId);
  void viewWillAppear(int w, int h) override;
  bool leftButtonUp(const ViewContext3D& viewContext) override;
  bool leftButtonDown(const ViewContext3D& viewContext) override;
  void render() const;
private:
  void setRandomImage();
};
}
