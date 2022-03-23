#include <optional>
#include <gtest/gtest.h>
#include <eigen3/Eigen/Dense>
#include <bgfx/bgfx.h>
#include "scene_model.h"
#include "views/add_keypoint_view.h"
#include <iostream>

std::string datasetPath;

TEST(TestAddKeypointTool, BasicCase) {
  fs::path path(datasetPath);
  SceneModel sceneModel((path / "scene" / "integrated.ply").string());
  sceneModel.loadMesh();
  Timeline timeline(sceneModel);
  views::AddKeypointView view(sceneModel, timeline);
  SceneCamera sceneCamera(path / "camera_intrinsics.json");
  ViewContext3D context(sceneCamera);
  context.camera.reset(Vector3f(0.0, 0.0, 0.0), Vector3f(0.0, 0.0, 0.0));
  context.camera.setPosition(Vector3f(0.0, 0.0, 0.2));
  context.camera.updateLookat(Vector3f(0.0, 0.0, 0.0));
  context.mousePositionX = 250;
  context.mousePositionY = 430;
  context.width = 500;
  context.height = 500;
  view.mouseMoved(context);
  bool added = view.leftButtonUp(context);
  ASSERT_TRUE(added);
  ASSERT_EQ(timeline.size(), 1);
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  bgfx::Init init;
  init.type = bgfx::RendererType::Noop;
  bgfx::init(init);
  datasetPath = argv[1];
  return RUN_ALL_TESTS();
}
