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
  int bottomRightIndex;
  Rectangle rectangle;
public:
  AddRectangleCommand(const std::array<Vector3f, 4> vs) : vertices(vs) {
    rectangle.topLeft = vertices[0];
    bottomRightIndex = findBottomRight();
    rectangle.bottomRight = vertices[bottomRightIndex];
    rectangle.normal = computeNormal();
  };

  void execute(SceneModel& sceneModel) override {
    sceneModel.addRectangle(rectangle);
  }

  void undo(SceneModel& sceneModel) override {
    sceneModel.removeRectangle(rectangle.id);
  };

private:
  int findBottomRight() {
    std::vector<float> distances;
    std::transform(vertices.begin() + 1, vertices.end(), std::back_inserter(distances), [&](const Vector3f& v) -> float {
      return -(v - rectangle.topLeft).norm();
    });
    return std::min_element(distances.begin(), distances.end()) - distances.begin() + 1;
  }

  Vector3f computeNormal() {
    Vector3f v1, v2;
    if (bottomRightIndex == 1) {
      v1 = vertices[2];
      v2 = vertices[3];
    } else if (bottomRightIndex == 2) {
      v1 = vertices[1];
      v2 = vertices[3];
    } else {
      v1 = vertices[1];
      v2 = vertices[2];
    }

    auto edge1 = (v1 - rectangle.topLeft);
    auto edge2 = (v2 - rectangle.topLeft);
    return edge2.cross(edge1);
  }
};

} // namespace commands
