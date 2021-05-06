#pragma once
#include <memory>
#include "timeline.h"
#include "views/mesh_view.h"
#include "views/view.h"
#include "scene_model.h"
#include "view_context_3d.h"
#include "glfw_app.h"
#include "controllers/controller.h"
#include "views/annotation_view.h"
#include "views/add_keypoint_view.h"
#include "views/move_keypoint_view.h"
#include "views/add_bbox_view.h"

class StudioViewController {
private:
  SceneModel& sceneModel;
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
  views::AddBBoxView addBBoxView;

public:
  StudioViewController(SceneModel& model, Timeline& timeline);
  void viewWillAppear(int width, int height);

  void render() const;

  bool leftButtonDown(double x, double y, InputModifier mod);
  bool leftButtonUp(double x, double y, InputModifier mod);
  bool mouseMoved(double x, double y, InputModifier mod);
  bool scroll(double xoffset, double yoffset, InputModifier mod);
  bool keypress(char character, InputModifier mod);
  void resize(int width, int height, InputModifier mod);

private:
  views::View3D& getActiveToolView();
};
