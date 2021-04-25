#include <optional>
#include <gtest/gtest.h>
#include <eigen3/Eigen/Dense>
#include <bgfx/bgfx.h>
#include "scene_model.h"
#include "tools/move_keypoint_tool.h"
#include "commands/keypoints.h"
#include "controllers/studio_view_controller.h"

std::string datasetPath;

using namespace tools;

TEST(TestAddKeypointTool, BasicCase) {
  SceneModel sceneModel(datasetPath);
  CommandStack stack;
  StudioViewController controller(sceneModel, stack);
  MoveKeypointTool tool(sceneModel, controller, controller.annotationController, stack);
  Camera camera(Vector3f::Zero(), 1.0);
  camera.updatePosition(Vector3f(0.0, 0.0, 0.2));
  camera.updateLookat(Vector3f(0.0, 0.0, 0.0));
  ViewContext3D context(camera);
  context.mousePositionX = 250;
  context.mousePositionY = 430;
  context.width = 500;
  context.height = 500;

  bool hit = tool.leftButtonDown(context);
  ASSERT_FALSE(hit);

  auto pointingAt = sceneModel.traceRay(camera.getPosition(), camera.computeRayWorld(context.width, context.height,
        context.mousePositionX, context.mousePositionY));
  auto command  = std::make_unique<commands::AddKeypointCommand>(pointingAt.value());
  command->execute(controller, sceneModel);
  stack.push_back(std::move(command));

  hit = tool.leftButtonDown(context);
  ASSERT_TRUE(hit);

  context.mousePositionX = 400;
  hit = tool.mouseMoved(context);
  ASSERT_TRUE(hit);
  hit = tool.leftButtonUp(context);
  ASSERT_TRUE(hit);
  ASSERT_EQ(stack.size(), 2);

  auto diff = pointingAt.value().transpose() - sceneModel.getKeypoints()[0].position.transpose();
  ASSERT_NE(sceneModel.getKeypoints()[0].position, pointingAt.value());
  ASSERT_GT(diff.norm(), 1e-2);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  bgfx::Init init;
  init.type = bgfx::RendererType::Noop;
  bgfx::init(init);
  datasetPath = argv[1];
  return RUN_ALL_TESTS();
}
