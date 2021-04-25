#include <gtest/gtest.h>
#include "scene_model.h"
#include "controllers/studio_view_controller.h"
#include "commands/keypoints.h"
#include <bgfx/bgfx.h>

std::string datasetPath;

using namespace commands;

TEST(TestAddKeypointApplyUndo, BasicCases) {
  SceneModel sceneModel(datasetPath);
  CommandStack stack;
  StudioViewController view(sceneModel, stack);
  view.viewWillAppear(500, 500);
  AddKeypointCommand command(Vector3f(1.0, 1.0, 1.0));
  ASSERT_EQ(view.meshView->getObjects().size(), 1);
  command.execute(view, sceneModel);
  ASSERT_EQ(sceneModel.getKeypoints().size(), 1);
  ASSERT_EQ(view.meshView->getObjects().size(), 1);

  command.undo(view, sceneModel);
  ASSERT_EQ(sceneModel.getKeypoints().size(), 0);
  ASSERT_EQ(view.meshView->getObjects().size(), 1);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  bgfx::Init init;
  init.type = bgfx::RendererType::Noop;
  bgfx::init(init);
  datasetPath = argv[1];
  return RUN_ALL_TESTS();
}
