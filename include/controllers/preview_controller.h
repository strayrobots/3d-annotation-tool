#include <memory>
#include "controllers/controller.h"
#include "views/image_pane.h"
#include "views/annotation_view.h"
#include "view_context_3d.h"
#include "scene_model.h"

namespace controllers {
class PreviewController : public controllers::Controller {
private:
  int viewId;
  std::unique_ptr<views::ImagePane> imageView;
  std::unique_ptr<views::AnnotationView> annotationView;
  const SceneModel& model;
  ViewContext3D viewContext;
public:
  PreviewController(const SceneModel& model, int viewId);
  void viewWillAppear(const views::Rect& rect) override;
  bool leftButtonUp(const ViewContext3D& viewContext) override;
  bool leftButtonDown(const ViewContext3D& viewContext) override;
  void render() const;
private:
  void setRandomImage();
};
}
