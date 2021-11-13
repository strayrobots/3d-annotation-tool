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

class MoveRectangleCommand : public Command {
private:
  const Rectangle rectangle;
  Vector3f oldPosition;
  Vector3f newPostion;
public:
  MoveRectangleCommand(Rectangle rectangle, Vector3f oldCenter, Vector3f newCenter) :
    rectangle(rectangle), oldPosition(oldCenter), newPostion(newCenter) {}

  void execute(SceneModel& sceneModel) override {
    Rectangle rect(rectangle);
    rect.center = newPostion;
    sceneModel.updateRectangle(rect);
  }

  void undo(SceneModel& sceneModel) override {
    Rectangle rect(rectangle);
    rect.center = oldPosition;
    sceneModel.updateRectangle(rect);
  }

};
} // namespace commands
