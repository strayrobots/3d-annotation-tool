#include <gtest/gtest.h>
#include <memory>
#include "timeline.h"
#include "scene_model.h"
#include "controllers/studio_view_controller.h"
#include "commands/keypoints.h"
#include "commands/move_keypoint_command.h"
#include <bgfx/bgfx.h>

std::string datasetPath;

using namespace commands;

TEST(TestMoveKeypointApplyUndo, BasicCases) {
  SceneModel sceneModel(datasetPath);
  Timeline timeline(sceneModel);
  Keypoint kp(1, 3, Vector3f::Ones());
  auto command = std::make_unique<AddKeypointCommand>(kp);
  timeline.pushCommand(std::move(command));
  ASSERT_EQ(kp.classId, 3);
  ASSERT_EQ(sceneModel.getKeypoints().size(), 1);
  ASSERT_EQ(sceneModel.getKeypoints()[0].position, Vector3f::Ones());
  ASSERT_EQ(sceneModel.getKeypoints()[0].classId, 3);

  Vector3f newPosition(0.25, -0.25, 0.3);
  Keypoint newKp = Keypoint(sceneModel.getKeypoints()[0]);
  newKp.position = newPosition;
  auto moveCommand = std::make_unique<MoveKeypointCommand>(sceneModel.getKeypoints()[0], newKp);
  timeline.pushCommand(std::move(moveCommand));
  ASSERT_EQ(sceneModel.getKeypoints()[0].position, newPosition);
  timeline.undoCommand();
  ASSERT_NE(sceneModel.getKeypoints()[0].position, newPosition);
  ASSERT_EQ(sceneModel.getKeypoints()[0].position, Vector3f::Ones());
  ASSERT_EQ(sceneModel.getKeypoints()[0].classId, 3);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  bgfx::Init init;
  init.type = bgfx::RendererType::Noop;
  bgfx::init(init);
  datasetPath = argv[1];
  return RUN_ALL_TESTS();
}
