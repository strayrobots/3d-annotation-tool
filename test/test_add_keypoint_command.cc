#include <gtest/gtest.h>
#include "scene_model.h"
#include "commands/keypoints.h"
#include "timeline.h"
#include <bgfx/bgfx.h>

std::string datasetPath;

using namespace commands;

TEST(TestAddKeypointApplyUndo, BasicCases) {
  SceneModel sceneModel(datasetPath);
  Timeline timeline(sceneModel);
  auto command = std::make_unique<AddKeypointCommand>(Vector3f(1.0, 1.0, 1.0));
  timeline.pushCommand(std::move(command));
  ASSERT_EQ(sceneModel.getKeypoints().size(), 1);
  auto command2 = std::make_unique<AddKeypointCommand>(Vector3f(0.2, 1.0, 1.0));
  timeline.pushCommand(std::move(command2));
  ASSERT_EQ(sceneModel.getKeypoints().size(), 2);

  timeline.undoCommand();
  ASSERT_EQ(sceneModel.getKeypoints().size(), 1);
  timeline.undoCommand();
  ASSERT_EQ(sceneModel.getKeypoints().size(), 0);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  datasetPath = argv[1];
  return RUN_ALL_TESTS();
}
