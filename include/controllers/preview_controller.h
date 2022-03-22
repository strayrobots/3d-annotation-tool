#include <memory>
#include "controllers/controller.h"
#include "views/image_pane.h"
#include "views/annotation_view.h"
#include "view_context_3d.h"
#include "scene_model.h"
#include <filesystem>

namespace controllers {
class PreviewController : public controllers::Controller3D {
private:
  int viewId;
  const SceneCamera& sceneCamera;
  const SceneModel& model;
  ViewContext3D viewContext;
  fs::path datasetPath;

  std::unique_ptr<views::ImagePane> imageView;
  std::unique_ptr<views::AnnotationView> annotationView;

public:
  PreviewController(const SceneModel& model, fs::path datasetPath, int viewId);
  void viewWillAppear(const views::Rect& rect) override;
  bool leftButtonUp(const ViewContext3D& viewContext) override;
  bool leftButtonDown(const ViewContext3D& viewContext) override;
  bool keypress(char keypress, const InputModifier mod) override;
  void render() const;

private:
  void setRandomImage();
  void setImage(float t);
  void setImage(int i);
};
} // namespace controllers
