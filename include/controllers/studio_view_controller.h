#pragma once
#include <memory>
#include <list>
#include "tools/tool.h"
#include "tools/add_keypoint_tool.h"
#include "tools/move_keypoint_tool.h"
#include "controllers/annotation_controller.h"
#include "views/mesh_view.h"
#include "views/view.h"
#include "scene_model.h"
#include "view_context_3d.h"
#include "glfw_app.h"
#include "controllers/controller.h"

class StudioViewController {
private:
  SceneModel& sceneModel;
  std::shared_ptr<views::MeshDrawable> meshDrawable;
  CommandStack& commandStack;

  // Changing view point.
  double prevX, prevY;
  bool dragging = false, moved = false;

  // Tools.
  std::shared_ptr<tools::AddKeypointTool> addKeypointTool;
  std::shared_ptr<tools::MoveKeypointTool> moveKeypointTool;
  std::shared_ptr<tools::Tool> currentTool;

  Camera camera;
  ViewContext3D viewContext;

public:
  // Subcontrollers
  controllers::AnnotationController annotationController;
  std::shared_ptr<views::MeshView> meshView;

  StudioViewController(SceneModel& model, CommandStack& stack);
  void viewWillAppear(int width, int height);

  void render() const;
  bool leftButtonDown(double x, double y, InputModifier mod);
  bool leftButtonUp(double x, double y, InputModifier mod);
  bool mouseMoved(double x, double y, InputModifier mod);
  bool scroll(double xoffset, double yoffset, InputModifier mod);
  bool keypress(char character, InputModifier mod);
  void resize(int width, int height, InputModifier mod);
};
