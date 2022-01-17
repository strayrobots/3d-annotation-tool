#pragma once
#include <memory>
#include "timeline.h"
#include "views/mesh_view.h"
#include "views/view.h"
#include "scene_model.h"
#include "view_context_3d.h"
#include "controllers/controller.h"
#include "controllers/preview_controller.h"
#include "views/point_cloud_view.h"
#include "views/annotation_view.h"
#include "views/add_keypoint_view.h"
#include "views/move_tool_view.h"
#include "views/add_bbox_view.h"
#include "views/status_bar_view.h"
#include "views/add_rectangle_view.h"

class StudioViewController : public controllers::Controller {
private:
  int viewId;
  SceneModel& sceneModel;
  std::pair<int, int> imageSize;

  // Changing view point.
  double prevX, prevY;
  bool dragging = false, moved = false;

  ViewContext3D viewContext;
  views::AnnotationView annotationView;
  views::MeshDrawable sceneMeshView;
  views::PointCloudView pointCloudView;

  // Tool views.
  views::AddKeypointView addKeypointView;
  views::MoveToolView moveToolView;
  views::AddBBoxView addBBoxView;
  views::AddRectangleView addRectangleView;

  views::StatusBarView statusBarView;

  // Sub-controllers
  std::shared_ptr<controllers::PreviewController> preview;
public:
  StudioViewController(SceneModel& model, Timeline& timeline);
  void viewWillAppear(const views::Rect& r) override;

  void render() const;
  void refresh();

  bool leftButtonDown(double x, double y, InputModifier mod);
  bool leftButtonUp(double x, double y, InputModifier mod);
  bool mouseMoved(double x, double y, InputModifier mod);
  bool scroll(double xoffset, double yoffset, InputModifier mod);
  bool keypress(char character, const InputModifier mod) override;
  void resize(const views::Rect& r) override;
private:
  views::View3D& getActiveToolView();
  views::Rect previewRect() const;
  views::Rect statusBarRect() const;
  void updateViewContext(double x, double y, InputModifier mod);
};
