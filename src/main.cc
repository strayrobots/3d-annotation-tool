#include <memory>
#include <thread>
#include <chrono>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include "3rdparty/nanort.h"
#include "views/mesh_view.h"
#include <3rdparty/json.hpp>
#include "glfw_app.h"

class LabelStudio : public GLFWApp {
private:
  std::unique_ptr<nanort::TriangleMesh<float>> nanoMesh;
  std::unique_ptr<nanort::TriangleSAHPred<float>> triangle_pred;
  nanort::BVHAccel<float> bvh;
  std::shared_ptr<views::MeshView> meshView;
  std::shared_ptr<views::TriangleMesh> mesh;

  // Changing view point.
  double prevX, prevY;
  bool dragging = false, moved = false;
  views::Camera camera;

  // Keypoints.
  bool pointingAtMesh = false;
  std::vector<Eigen::Vector3f> keypoints;
  Eigen::Vector3f pointingAt = Eigen::Vector3f::Zero();
public:

  LabelStudio() : GLFWApp("LabelStudio"), camera(Vector3f(0.0, 1.0, 0.0)) {
    meshView = std::make_shared<views::MeshView>();
    mesh = std::make_shared<views::Mesh>("../bunny.ply");
    meshView->addObject(mesh);

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
      LabelStudio* w = (LabelStudio*)glfwGetWindowUserPointer(window);
      if((GLFW_MOD_CONTROL == mods) && (GLFW_KEY_S == key))  {
        LabelStudio* w = (LabelStudio*)glfwGetWindowUserPointer(window);
        nlohmann::json json = nlohmann::json::array();
        for (size_t i = 0; i < w->keypoints.size(); i++) {
          json[i] = { {"x", w->keypoints[i][0]}, {"y", w->keypoints[i][1]}, {"z", w->keypoints[i][2]} };
        }
        std::ofstream file("keypoints.json");
        file << json;
        std::cout << "Saved keypoints to keypoints.json" << std::endl;
      }
    });
  }

  void leftButtonDown() {
    dragging = true;
    moved = false;
    glfwGetCursorPos(window, &prevX, &prevY);
  }

  void leftButtonUp() {
    dragging = false;
    if (!moved && pointingAtMesh) {
      keypoints.push_back(pointingAt);

      Matrix4f T = Matrix4f::Identity();
      T.block(0, 3, 3, 1) = pointingAt;
      auto sphere = std::make_shared<views::Sphere>(T, 0.005, Vector4f(1.0, 0.5, 0.5, 1.0));
      meshView->addObject(sphere);
      std::cout << "Added keypoint: " << pointingAt.transpose() << std::endl;
    }
  }

  void mouseMoved(double x, double y) {
    moved = true;
    if (dragging) {
      float diffX = (x - prevX) * M_PI / 1000.0;
      float diffY = (y - prevY) * M_PI / 1000.0;
      Quaternionf rotationX, rotationY;
      rotationY = AngleAxis(diffY, camera.getOrientation() * Vector3f::UnitX());
      rotationX = AngleAxis(diffX, camera.getOrientation() * Vector3f::UnitY());
      camera.setOrientation(rotationX * rotationY * camera.getOrientation());

      prevX = x;
      prevY = y;
    }
    nanort::Ray<float> ray;
    ray.min_t = 0.0;
    ray.max_t = 1e9f;

    float aspectRatio = width / height;
    float fov = camera.fov;
    float pX = (2.0f * (x / width) - 1.0f) * std::tan(fov / 2.0f * M_PI / 180) * aspectRatio;
    float pY = (1.0f - 2.0f * (y / height)) * std::tan(fov / 2.0f * M_PI / 180);
    Vector3f cameraRay(pX, pY, 1.0f);
    cameraRay = camera.getOrientation() * cameraRay.normalized();

    Vector3f rayOrigin = camera.getPosition();
    ray.org[0] = rayOrigin[0];
    ray.org[1] = rayOrigin[1];
    ray.org[2] = rayOrigin[2];

    ray.dir[0] = cameraRay[0];
    ray.dir[1] = cameraRay[1];
    ray.dir[2] = cameraRay[2];
    const auto& faces = mesh->indices();
    nanort::TriangleIntersector<float, nanort::TriangleIntersection<float>> triangleIntersector(mesh->vertices().data(), faces.data(), sizeof(float) * 3);
    nanort::TriangleIntersection<float> isect;
    pointingAtMesh = bvh.Traverse(ray, triangleIntersector, &isect);
    if (pointingAtMesh) {
      uint32_t faceId = isect.prim_id;
      const auto& face = faces.row(faceId);
      const Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor>& vertices = mesh->vertices();
      auto vertex1 = vertices.row(face[0]);
      auto vertex2 = vertices.row(face[1]);
      auto vertex3 = vertices.row(face[2]);
      Vector3f point = (1.0f - isect.u - isect.v) * vertex1 + isect.u * vertex2 + isect.v * vertex3;
      pointingAt = point;
      std::cout << "pointingAt: " << pointingAt.transpose() << std::endl;
    }
  }

  void scroll(double xoffset, double yoffset) {
    (void)xoffset;
    double diff = yoffset * 0.05;
    const auto& cameraPosition = camera.getPosition();
    double newNorm = std::max(cameraPosition.norm() + diff, 0.1);
    camera.setPosition(newNorm * cameraPosition.normalized());
  }

  bool update() const override {
    bgfx::setDebug(BGFX_DEBUG_TEXT);

    glfwWaitEventsTimeout(0.02);
    bgfx::setViewRect(0, 0, 0, uint16_t(width), uint16_t(height));
    view->render(camera);

    bgfx::frame();

    return !glfwWindowShouldClose(window);
  }

private:
  void initRayTracing() {
    auto faces = mesh->indices();
    const float* vertices = mesh->vertices().data();
    const uint32_t* indices = faces.data();
    nanoMesh = std::make_unique<nanort::TriangleMesh<float>>(vertices, indices, sizeof(float) * 3);
    triangle_pred = std::make_unique<nanort::TriangleSAHPred<float>>(vertices, indices, sizeof(float) * 3);
    nanort::BVHBuildOptions<float> build_options;
    std::cout << "Building bounding volume hierarchy.\r" << std::flush;
    auto ret = bvh.Build(faces.rows(), *nanoMesh.get(), *triangle_pred.get(), build_options);
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
