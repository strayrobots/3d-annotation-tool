#include <gtest/gtest.h>
#include "scene_model.h"
#include "utils/dataset.h"

std::string datasetPath;

TEST(TestAddKeypointAddRemove, BasicCase) {
  SceneModel model(datasetPath);
  Keypoint kp1(-1, 1, Vector3f::Ones());
  kp1 = model.addKeypoint(kp1);
  ASSERT_TRUE(kp1.id > 0);
  ASSERT_EQ(kp1.position[0], 1.0f);
  ASSERT_EQ(kp1.position[1], 1.0f);
  ASSERT_EQ(kp1.position[2], 1.0f);
  ASSERT_EQ(kp1.classId, 1);
  ASSERT_EQ(model.getKeypoints().size(), 1);
  auto kp2 = model.addKeypoint(Vector3f(1.0, 1.0, 1.0));
  auto kp3 = model.addKeypoint(Vector3f(1.0, 1.0, 1.0));
  ASSERT_EQ(model.getKeypoints().size(), 3);

  model.removeKeypoint(kp1);
  model.removeKeypoint(kp2);
  model.removeKeypoint(kp3);
  ASSERT_EQ(model.getKeypoints().size(), 0);
  ASSERT_EQ(model.getKeypoints()[1].id, 3);

  BBox bbox = {.id = -1, .position = Vector3f::Ones()};
  model.addBoundingBox(bbox);
  ASSERT_EQ(bbox.id, 1);
  ASSERT_EQ(model.getBoundingBoxes().size(), 1);

  bbox.orientation = Quaternionf(0.25, 0.25, 0.25, 0.25).normalized();
  model.updateBoundingBox(bbox);
  auto updated = model.getBoundingBox(bbox.id).value();
  ASSERT_EQ(updated.orientation.w(), bbox.orientation.w());
  ASSERT_EQ(updated.orientation.x(), bbox.orientation.x());
  ASSERT_EQ(updated.orientation.y(), bbox.orientation.y());
  ASSERT_EQ(updated.orientation.z(), bbox.orientation.z());
  model.removeBoundingBox(bbox.id);
  ASSERT_EQ(model.getBoundingBoxes().size(), 0);
}

TEST(TestUpdateKeypoint, BasicCase) {
  SceneModel model(datasetPath);
  auto kp1 = model.addKeypoint(Vector3f(1.0, 1.0, 1.0));
  Keypoint kp2(kp1.id, Vector3f::Zero());
  kp2.classId = 3;
  model.updateKeypoint(kp1.id, kp2);
  ASSERT_EQ(model.getKeypoint(kp1.id).value().position, Vector3f::Zero());
  ASSERT_EQ(model.getKeypoint(kp1.id).value().id, kp2.id);
  ASSERT_EQ(model.getKeypoint(kp1.id).value().classId, 3);
}

TEST(SceneModelTest, Camera) {
  SceneModel model(datasetPath);
  auto trajectory = utils::dataset::getDatasetCameraTrajectory((datasetPath / "scene" / "trajectory.log"));
  ASSERT_EQ(trajectory.size(), 474);
  ASSERT_EQ(trajectory[0], Matrix4f::Identity());
  ASSERT_NE(trajectory[1], Matrix4f::Identity());
  ASSERT_NEAR(trajectory[1](0, 0), 0.999, 0.001);
  ASSERT_NEAR(trajectory[1](0, 1), 0.00386, 0.001);
  ASSERT_NEAR(trajectory[1](1, 1), 0.999, 0.001);
  ASSERT_NEAR(trajectory[1](3, 3), 1.0, 0.001);
  ASSERT_NEAR(trajectory[1](3, 0), 0.0, 0.001);
  ASSERT_NEAR(trajectory[1](3, 1), 0.0, 0.001);
  // auto camera = model.sceneCamera(); TODO: fix
  // ASSERT_NEAR(camera.fov, 52.6131, 0.1); TODO: fix
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  datasetPath = argv[1];
  return RUN_ALL_TESTS();
}
