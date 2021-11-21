#pragma once
#include <algorithm>
#include <eigen3/Eigen/Dense>
#include "commands/command.h"
#include "scene_model.h"
#include <cassert>

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

class EditRectangleCommand : public Command {
private:
  const Rectangle oldRectangle;
  const Rectangle newRectangle;
public:
  EditRectangleCommand(Rectangle rectangle, Rectangle newRectangle) :
    oldRectangle(rectangle), newRectangle(newRectangle) {
      assert(rectangle.id == newRectangle.id);
    }

  void execute(SceneModel& sceneModel) override {
    sceneModel.updateRectangle(newRectangle);
  }

  void undo(SceneModel& sceneModel) override {
    sceneModel.updateRectangle(oldRectangle);
  }

};
} // namespace commands
