#pragma once
#include <memory>
#include <list>
#include "views/mesh_view.h"
#include "views/view.h"
#include "scene_model.h"
#include "commands/command.h"

class StudioViewController : public views::View {
private:
  SceneModel& sceneModel;
  std::shared_ptr<views::MeshDrawable> meshDrawable;
  std::list<std::unique_ptr<commands::Command>> commandStack;

  std::optional<Vector3f> pointingAt;
  // Changing view point.
  double prevX, prevY;
  bool dragging = false, moved = false;
public:
  std::shared_ptr<views::MeshView> meshView;

  StudioViewController(SceneModel& model);
  void render(const Camera& camera) const override;
  void leftButtonDown(double x, double y);
  void leftButtonUp(double x, double y);
  void mouseMoved(double x, double y);
  // Commands.
  void undo();
  void pushCommand(std::unique_ptr<commands::Command>);
};

