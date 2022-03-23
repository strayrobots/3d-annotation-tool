#include <gtest/gtest.h>
#include "camera.h"

TEST(TestCamera, Init) {
  Camera camera;
  camera.reset(Vector3f(1.0, 1.0, 1.0), Vector3f(-2.0, -2.0, -2.0));
  ASSERT_EQ(camera.getLookat(), Vector3f(1.0, 1.0, 1.0));
  ASSERT_EQ(camera.getPosition(), Vector3f(-2.0, -2.0, -2.0));
  Matrix3f expectedInitialOrientation;
  expectedInitialOrientation << 0.707107, 0.408248, -0.57735,
      2.98023e-08, -0.816496, -0.57735,
      -0.707107, 0.408248, -0.57735;

  Affine3f expectedInitialViewMatrix;
  expectedInitialViewMatrix.translation() = Vector3f(2.38419e-07, 1.19209e-07, -3.4641);
  expectedInitialViewMatrix.linear() << 0.707107, 2.98023e-08, -0.707107,
      0.408248, -0.816496, 0.408248,
      -0.57735, -0.57735, -0.57735;

  ASSERT_TRUE(camera.getOrientation().toRotationMatrix().isApprox(expectedInitialOrientation));
  ASSERT_TRUE(camera.getViewMatrix().isApprox(expectedInitialViewMatrix));
}

TEST(TestCamera, CameraMatrixInit) {
  Matrix3f cameraMatrix;
  cameraMatrix << 100.0, 0.0, 50.0,
      0.0, 100.0, 50.0,
      0.0, 0.0, 0.0;
  SceneCamera sceneCamera(cameraMatrix, 100, 100);
  Camera camera(sceneCamera);
  ASSERT_NEAR(camera.fov, 53.1301, 0.01);

  // Test moving camera.
  Vector3f p(0.25, 0.52, 0.1);
  camera.setPosition(p);
  ASSERT_EQ(camera.getPosition(), p);
  Quaternionf orientation = Quaternionf(0.25, 0.25, 0.25, 0.25).normalized();
  camera.setOrientation(orientation);
  ASSERT_EQ(camera.getOrientation().w(), orientation.w());
  ASSERT_EQ(camera.getOrientation().x(), orientation.x());
  ASSERT_EQ(camera.getOrientation().y(), orientation.y());
  ASSERT_EQ(camera.getOrientation().z(), orientation.z());
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
