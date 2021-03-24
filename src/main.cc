#include <memory>
#include <thread>
#include <chrono>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/bx.h>
#include <bx/mutex.h>
#include <bx/thread.h>
#include "3rdparty/nanort.h"
#include "views/mesh_view.h"
#include <3rdparty/json.hpp>
#include "glfw_app.h"

int renderThread(bx::Thread *thread, void* userData) {
  return 0;
}


class LabelStudio : public GLFWApp {
private:
  bool dragging = false;
  double mouseDownX, mouseDownY;
  std::unique_ptr<nanort::TriangleMesh<float>> mesh;
  std::unique_ptr<nanort::TriangleSAHPred<float>> triangle_pred;
  nanort::BVHAccel<float> bvh;
  std::shared_ptr<views::MeshView> meshView;

  Eigen::Matrix3f currentRotation = Eigen::Matrix3f::Identity();
  Eigen::Matrix3f rotationStart = Eigen::Matrix3f::Identity();
  Eigen::Vector3f eyePos = Eigen::Vector3f(0.0, 0.0, -1.0);

  std::vector<Eigen::Vector3f> keypoints;
public:
  LabelStudio() : GLFWApp("LabelStudio") {
    meshView = std::make_shared<views::MeshView>("../bunny.ply");

    setView(meshView);
    glfwSetMouseButtonCallback(window, [](GLFWwindow *window, int button, int action, int mods) {
      LabelStudio* w = (LabelStudio*)glfwGetWindowUserPointer(window);
      if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        w->leftButtonDown();
      } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        w->leftButtonUp();
      }
    });
    glfwSetCursorPosCallback(window, [](GLFWwindow *window, double xpos, double ypos) {
      LabelStudio* w = (LabelStudio*)glfwGetWindowUserPointer(window);
      w->mouseMoved(xpos, ypos);
    });

    glfwSetScrollCallback(window, [](GLFWwindow *window, double xoffset, double yoffset) {
      LabelStudio* w = (LabelStudio*)glfwGetWindowUserPointer(window);
      w->scroll(xoffset, yoffset);
    });

    initRayTracing();
    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
      if((GLFW_MOD_CONTROL == mods) && (GLFW_KEY_S == key))  {
        LabelStudio* w = (LabelStudio*)glfwGetWindowUserPointer(window);
        nlohmann::json json;
        for (size_t i = 0; i < w->keypoints.size(); i++) {
          json[std::to_string(i)] = { {"x", w->keypoints[i][0]}, {"y", w->keypoints[i][1]}, {"z", w->keypoints[i][2]} };
        }
        std::ofstream file("keypoints.json");
        file << json;
        std::cout << "Saved keypoints to keypoints.json" << std::endl;
      }
    });
  }

  void leftButtonDown() {
    dragging = true;
    glfwGetCursorPos(window, &mouseDownX, &mouseDownY);
    rotationStart = currentRotation;
  }

  void leftButtonUp() {
    dragging = false;
  }

  void mouseMoved(double x, double y) {
    if (dragging) {
      double diff_x = (x - mouseDownX) / width;
      double diff_y = (y - mouseDownY) / height;
      Matrix3f rotation;
      rotation = AngleAxisf(diff_x * M_PI, Vector3f::UnitY()) * AngleAxisf(diff_y * M_PI, Vector3f::UnitX());
      currentRotation = rotation * rotationStart;
    }
    nanort::Ray<float> ray;
    ray.min_t = 0.0;
    ray.max_t = 1e9f;

    float aspectRatio = width / height;
    float fov = meshView->fov;
    float pX = (2.0f * (x / width) - 1.0f) * std::tan(fov / 2.0f * M_PI / 180) * aspectRatio;
    float pY = (1.0f - 2.0f * (y / height)) * std::tan(fov / 2.0f * M_PI / 180);
    Vector3f cameraRay(pX, pY, 1.0f);
    cameraRay.normalize();
    cameraRay = currentRotation.transpose() * cameraRay;

    Vector3f rayOrigin = currentRotation.transpose() * eyePos;

    ray.org[0] = rayOrigin[0];
    ray.org[1] = rayOrigin[1];
    ray.org[2] = rayOrigin[2];

    ray.dir[0] = cameraRay[0];
    ray.dir[1] = cameraRay[1];
    ray.dir[2] = cameraRay[2];
    nanort::TriangleIntersector<float, nanort::TriangleIntersection<float>> triangleIntersector(meshView->vertices().data(), meshView->indices().data(), sizeof(float) * 3);
    nanort::TriangleIntersection<float> isect;
    const bool hit = bvh.Traverse(ray, triangleIntersector, &isect);
    if (hit) {
      std::cout << "hit mesh" << std::endl;
    }
  }

  void scroll(double xoffset, double yoffset) {
    (void)xoffset;
    double diff = yoffset * 0.05;
    eyePos[2] = std::min(eyePos[2] + diff, -0.1);
  }

  bool update() const override {
    bgfx::setDebug(BGFX_DEBUG_TEXT);

    glfwWaitEventsTimeout(0.016);
    bgfx::setViewRect(0, 0, 0, uint16_t(width), uint16_t(height));
    view->render(eyePos, currentRotation);

    bgfx::frame();

    return !glfwWindowShouldClose(window);
  }

private:
  void initRayTracing() {
    auto faces = meshView->indices();
    const float* vertices = meshView->vertices().data();
    const uint32_t* indices = faces.data();
    mesh = std::make_unique<nanort::TriangleMesh<float>>(vertices, indices, sizeof(float) * 3);
    triangle_pred = std::make_unique<nanort::TriangleSAHPred<float>>(vertices, indices, sizeof(float) * 3);
    nanort::BVHBuildOptions<float> build_options;
    std::cout << "Building bounding volume hierarchy.\r" << std::flush;
    auto ret = bvh.Build(faces.rows(), *mesh.get(), *triangle_pred.get(), build_options);
    assert(ret);
    std::cout << "Done building bounding volume hierarchy." << std::endl;
    nanort::BVHBuildStatistics stats = bvh.GetStatistics();
  }
};

int main(void) {
  auto window = std::make_shared<LabelStudio>();

  while (window->update()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
  }

  window = nullptr;

  return 0;
}
