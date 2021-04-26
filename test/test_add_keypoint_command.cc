#include <gtest/gtest.h>
#include "scene_model.h"
#include "controllers/studio_view_controller.h"
#include "commands/keypoints.h"
#include "timeline.h"
#include <bgfx/bgfx.h>

std::string datasetPath;

using namespace commands;

TEST(TestAddKeypointApplyUndo, BasicCases) {
  SceneModel sceneModel(datasetPath);
  Timeline timeline(sceneModel);
  StudioViewController view(sceneModel, timeline);
  view.viewWillAppear(500, 500);
  auto command = std::make_unique<AddKeypointCommand>(Vector3f(1.0, 1.0, 1.0));
  ASSERT_EQ(view.meshView->getObjects().size(), 1);
  timeline.pushCommand(std::move(command));
  ASSERT_EQ(sceneModel.getKeypoints().size(), 1);

  timeline.undoCommand();
  ASSERT_EQ(sceneModel.getKeypoints().size(), 0);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  bgfx::Init init;
  init.type = bgfx::RendererType::Noop;
  bgfx::init(init);
  datasetPath = argv[1];
  return RUN_ALL_TESTS();
}
