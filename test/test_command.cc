#include <gtest/gtest.h>
#include "scene_model.h"
#include "studio_view.h"
#include "commands/keypoints.h"
#include <bgfx/bgfx.h>

std::string datasetPath;

using namespace commands;

TEST(TestAddKeypointApplyUndo, BasicCases) {
  SceneModel sceneModel(datasetPath);
  StudioView view(sceneModel);
  AddKeypointCommand command(Vector3f(1.0, 1.0, 1.0));
  ASSERT_EQ(view.meshView->getObjects().size(), 1);
  command.execute(view, sceneModel);
  ASSERT_EQ(sceneModel.getKeypoints().size(), 1);
  ASSERT_EQ(view.meshView->getObjects().size(), 2);

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
