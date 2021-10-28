#include <gtest/gtest.h>
#include "scene_model.h"
#include "commands/rectangle.h"
#include "timeline.h"
#include <bgfx/bgfx.h>

std::string datasetPath;

using namespace commands;

TEST(TestApplyUndoAddRectangle, BasicCase) {
  SceneModel sceneModel(datasetPath);
  Timeline timeline(sceneModel);

  std::array<Vector3f, 4> vertices = {
    Vector3f(0.0f, 0.0f, 0.0f),
    Vector3f(0.0f, 1.0f, 0.0f),
    Vector3f(1.0f, 0.0f, 0.0f),
    Vector3f(1.0f, 1.0f, 0.0f),
  };
  auto command = std::make_unique<AddRectangleCommand>(vertices, 1);
  timeline.pushCommand(std::move(command));
  ASSERT_EQ(sceneModel.getRectangles().size(), 1);

  auto center = Vector3f(0.5f, 0.5f, 0.0f);
  ASSERT_TRUE(sceneModel.getRectangles()[0].center == center);
  ASSERT_TRUE(sceneModel.getRectangles()[0].orientation.w() == 1.0f);
  ASSERT_TRUE(sceneModel.getRectangles()[0].normal() == Vector3f::UnitZ());
  ASSERT_EQ(sceneModel.getRectangles()[0].classId, 1);

  // Check undo.
  timeline.undoCommand();
  ASSERT_EQ(sceneModel.getRectangles().size(), 0);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  datasetPath = argv[1];
  return RUN_ALL_TESTS();
}
