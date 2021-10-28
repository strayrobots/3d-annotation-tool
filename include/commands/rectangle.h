#pragma once
#include <algorithm>
#include <eigen3/Eigen/Dense>
#include "commands/command.h"
#include "scene_model.h"

namespace commands {

using namespace Eigen;
class AddRectangleCommand : public Command {
private:
  const std::array<Vector3f, 4> vertices;
  int classId, bottomRightIndex;
  Rectangle rectangle;
public:
  AddRectangleCommand(const std::array<Vector3f, 4> vs, int classId) : vertices(vs), classId(classId), rectangle(vs) {
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
