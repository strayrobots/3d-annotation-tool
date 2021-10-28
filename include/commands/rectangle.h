#pragma once
#include <algorithm>
#include <eigen3/Eigen/Dense>
#include "commands/command.h"
#include "scene_model.h"

namespace commands {

using namespace Eigen;
class AddRectangleCommand : public Command {
private:
  Rectangle rectangle;
  int classId, bottomRightIndex;
public:
  AddRectangleCommand(const Rectangle& rectangle) : rectangle(rectangle), classId(rectangle.classId) {
  };
  AddRectangleCommand(const std::array<Vector3f, 4> vs, int classId) : rectangle(Rectangle(vs)), classId(classId) {
  };

  void execute(SceneModel& sceneModel) override {
    rectangle.classId = classId;
    sceneModel.addRectangle(rectangle);
  }

  void undo(SceneModel& sceneModel) override {
    sceneModel.removeRectangle(rectangle.id);
  };
};

} // namespace commands
