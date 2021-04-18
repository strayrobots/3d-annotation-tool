#include <gtest/gtest.h>
#include "camera.h"




TEST(TestAddKeypointApplyUndo, BasicCases) {
  Camera camera(Vector3f(1.0, 1.0, 1.0), -2.0);
  ASSERT_EQ(camera.getLookat(), Vector3f(1.0, 1.0, 1.0));
  ASSERT_EQ(camera.getPosition(), Vector3f(-2.0, -2.0, -2.0));
  Matrix3f expectedInitialOrientation;
  expectedInitialOrientation << 0.707107,    0.408248,    -0.57735,
                                2.98023e-08,   -0.816496,    -0.57735,
                                -0.707107,    0.408248,    -0.57735;

  Affine3f expectedInitialViewMatrix;
  expectedInitialViewMatrix.translation() = Vector3f(2.38419e-07, 1.19209e-07, -3.4641);
  expectedInitialViewMatrix.linear() << 0.707107, 2.98023e-08,   -0.707107,
                                        0.408248,   -0.816496,    0.408248,
                                        -0.57735,    -0.57735,    -0.57735;

  ASSERT_TRUE(camera.getOrientation().toRotationMatrix().isApprox(expectedInitialOrientation));
  ASSERT_TRUE(camera.getViewMatrix().isApprox(expectedInitialViewMatrix));

  

}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
