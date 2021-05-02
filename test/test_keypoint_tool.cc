#include <optional>
#include <gtest/gtest.h>
#include <eigen3/Eigen/Dense>
#include <bgfx/bgfx.h>
#include "scene_model.h"
#include "tools/add_keypoint_tool.h"
#include "controllers/studio_view_controller.h"

std::string datasetPath;

using namespace tools;

TEST(TestAddKeypointTool, BasicCase) {
  SceneModel sceneModel(datasetPath);
  CommandStack stack;
  StudioViewController controller(sceneModel, stack);
  AddKeypointTool tool(sceneModel, controller, stack);
  Camera camera;
  camera.reset(Vector3f(0.0, 0.0, 0.0), Vector3f(0.0, 0.0, 0.0));
  camera.updatePosition(Vector3f(0.0, 0.0, 0.2));
  camera.updateLookat(Vector3f(0.0, 0.0, 0.0));
  ViewContext3D context(camera);
  context.mousePositionX = 250;
  context.mousePositionY = 430;
  context.width = 500;
  context.height = 500;
  tool.mouseMoved(context);
  bool added = tool.leftButtonUp(context);
  ASSERT_TRUE(added);
  ASSERT_EQ(stack.size(), 1);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  bgfx::Init init;
  init.type = bgfx::RendererType::Noop;
  bgfx::init(init);
  datasetPath = argv[1];
  return RUN_ALL_TESTS();
}
