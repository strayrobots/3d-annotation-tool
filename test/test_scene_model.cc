#include <gtest/gtest.h>
#include "scene_model.h"

std::string datasetPath;

TEST(TestAddKeypointAddRemove, BasicCase) {
  SceneModel model(datasetPath);
  auto kp1 = model.addKeypoint(Vector3f(1.0, 1.0, 1.0));
  ASSERT_TRUE(kp1.id > 0);
  ASSERT_EQ(kp1.position[0], 1.0f);
  ASSERT_EQ(kp1.position[1], 1.0f);
  ASSERT_EQ(kp1.position[2], 1.0f);
  ASSERT_EQ(model.getKeypoints().size(), 1);
  auto kp2 = model.addKeypoint(Vector3f(1.0, 1.0, 1.0));
  auto kp3 = model.addKeypoint(Vector3f(1.0, 1.0, 1.0));
  ASSERT_EQ(model.getKeypoints().size(), 3);

  model.removeKeypoint(kp1);
  model.removeKeypoint(kp2);
  model.removeKeypoint(kp3);
  ASSERT_EQ(model.getKeypoints().size(), 0);
  ASSERT_EQ(model.getKeypoints()[1].id, 3);

  BBox bbox = { .id = -1, .position = Vector3f::Ones() };
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
  model.updateKeypoint(kp1.id, kp2);
  ASSERT_EQ(model.getKeypoint(kp1.id).value().position, Vector3f::Zero());
  ASSERT_EQ(model.getKeypoint(kp1.id).value().id, kp2.id);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  datasetPath = argv[1];
  return RUN_ALL_TESTS();
}
