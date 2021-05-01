#pragma once
#include <memory>
#include "timeline.h"
#include "views/mesh_view.h"
#include "views/view.h"
#include "scene_model.h"
#include "view_context_3d.h"
#include "controllers/controller.h"
#include "views/annotation_view.h"
#include "views/add_keypoint_view.h"
#include "views/move_keypoint_view.h"

class StudioViewController {
private:
  SceneModel& sceneModel;
  std::shared_ptr<views::MeshDrawable> meshDrawable;
  Timeline& timeline;

  // Changing view point.
  double prevX, prevY;
  bool dragging = false, moved = false;

  Camera camera;
  ViewContext3D viewContext;
  views::AnnotationView annotationView;
  views::MeshDrawable sceneMeshView;

  // Tool views.
  views::AddKeypointView addKeypointView;
  views::MoveKeypointView moveKeypointView;

public:
  StudioViewController(SceneModel& model, Timeline& timeline);
  void viewWillAppear(int width, int height);

  void render() const;

  bool leftButtonDown(double x, double y);
  bool leftButtonUp(double x, double y);
  bool mouseMoved(double x, double y);
  bool scroll(double xoffset, double yoffset);
  bool keypress(char character);
  views::View3D& getActiveToolView();

  void resize(int width, int height);
};
