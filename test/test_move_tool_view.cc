#include <optional>
#include <gtest/gtest.h>
#include <eigen3/Eigen/Dense>
#include <bgfx/bgfx.h>
#include "scene_model.h"
#include "views/move_tool_view.h"
#include "commands/keypoints.h"

std::string datasetPath;

TEST(TestMoveToolView, BasicCase) {
  fs::path path(datasetPath);
  SceneModel sceneModel((path / "scene" / "integrated.ply").string());
  sceneModel.loadMesh();
  Timeline timeline(sceneModel);
  views::MoveToolView view(sceneModel, timeline);

  SceneCamera sceneCamera(path / "camera_intrinsics.json");
  ViewContext3D context(sceneCamera);
  context.camera.reset(Vector3f(0.0, 0.0, 0.0), Vector3f(0.0, 0.0, 0.0));
  context.camera.setPosition(Vector3f(0.0, 0.0, 0.2));
  context.camera.updateLookat(Vector3f(0.0, 0.0, 0.0));
  context.mousePositionX = 250;
  context.mousePositionY = 400;
  context.width = 500;
  context.height = 500;

  bool hit = view.leftButtonDown(context);
  ASSERT_FALSE(hit);

  auto pointingAt = sceneModel.traceRay(context.camera.getPosition(), context.camera.computeRayWorld(context.width, context.height,
                                                                                                     context.mousePositionX, context.mousePositionY));
  ASSERT_TRUE(pointingAt.has_value());
  Keypoint kp(0, 1, pointingAt.value());
  auto command = std::make_unique<commands::AddKeypointCommand>(kp);
  timeline.pushCommand(std::move(command));

  hit = view.leftButtonDown(context);
  ASSERT_TRUE(hit);
  ASSERT_EQ(sceneModel.activeKeypoint, sceneModel.getKeypoints()[0].id);
  ASSERT_NE(sceneModel.activeKeypoint, -1);

  hit = view.leftButtonDown(context);
  ASSERT_TRUE(hit);

  context.mousePositionX = 400;
  hit = view.mouseMoved(context);
  ASSERT_TRUE(hit);
  hit = view.leftButtonUp(context);
  ASSERT_TRUE(hit);
  ASSERT_EQ(timeline.size(), 2);

  auto diff = pointingAt.value().transpose() - sceneModel.getKeypoints()[0].position.transpose();
  ASSERT_NE(sceneModel.getKeypoints()[0].position, pointingAt.value());
  ASSERT_GT(diff.norm(), 1e-2);

  sceneModel.currentClassId = 7;
  bool change = view.keypress('7', 0);
  ASSERT_TRUE(change);
  ASSERT_EQ(timeline.size(), 3);
  ASSERT_EQ(sceneModel.getKeypoints()[0].classId, 7);
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  bgfx::Init init;
  init.type = bgfx::RendererType::Noop;
  bgfx::init(init);
  datasetPath = argv[1];
  return RUN_ALL_TESTS();
}
