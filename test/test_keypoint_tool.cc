#include <optional>
#include <gtest/gtest.h>
#include <eigen3/Eigen/Dense>
#include "scene_model.h"
#include "tools/add_keypoint_tool.h"

std::string datasetPath;

using namespace tools;

TEST(TestKeypointTool, BasicCase) {
  const SceneModel sceneModel(datasetPath);
  AddKeypointTool tool(sceneModel);
  std::optional<Vector3f> pointingAt = Vector3f(1.0, 0.0, 1.0);
  auto command = tool.leftClick(pointingAt);
  ASSERT_TRUE(command.has_value());
  pointingAt = {};
  auto command2 = tool.leftClick(pointingAt);
  ASSERT_FALSE(command2.has_value());
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  datasetPath = argv[1];
  return RUN_ALL_TESTS();
}
