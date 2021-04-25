#pragma once
#include <memory>
#include <list>
#include "tools/tool.h"
#include "tools/add_keypoint_tool.h"
#include "views/mesh_view.h"
#include "views/view.h"
#include "scene_model.h"
#include "view_context_3d.h"
#include "commands/command.h"

class StudioViewController {
private:
  SceneModel& sceneModel;
  std::shared_ptr<views::MeshDrawable> meshDrawable;
  std::list<std::unique_ptr<commands::Command>> commandStack;

  std::optional<Vector3f> pointingAt;
  // Changing view point.
  double prevX, prevY;
  bool dragging = false, moved = false;

  // Tools.
  std::shared_ptr<tools::AddKeypointTool> addKeypointTool;
  std::shared_ptr<tools::Tool> currentTool;

  Camera camera;
  ViewContext3D viewContext;

public:
  std::shared_ptr<views::MeshView> meshView;

  StudioViewController(SceneModel& model);
  void viewWillAppear(int width, int height);

  void render() const;
  void leftButtonDown(double x, double y);
  void leftButtonUp(double x, double y);
  void mouseMoved(double x, double y);
  void scroll(double xoffset, double yoffset);
  void keypress(char character);
  void resize(int width, int height);
  // Commands.
  void undo();
  void pushCommand(std::unique_ptr<commands::Command>);
};
