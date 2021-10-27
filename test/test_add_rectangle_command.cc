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
  auto command = std::make_unique<AddRectangleCommand>(vertices);
  timeline.pushCommand(std::move(command));
  ASSERT_EQ(sceneModel.getRectangles().size(), 1);

  ASSERT_TRUE(sceneModel.getRectangles()[0].topLeft == vertices[0]);
  ASSERT_TRUE(sceneModel.getRectangles()[0].bottomRight == vertices[3]);
  ASSERT_TRUE(sceneModel.getRectangles()[0].normal == Vector3f::UnitZ());

  // Check undo.
  timeline.undoCommand();
  ASSERT_EQ(sceneModel.getRectangles().size(), 0);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  datasetPath = argv[1];
  return RUN_ALL_TESTS();
}
