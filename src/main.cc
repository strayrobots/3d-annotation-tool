#include <memory>
#include <thread>
#include <chrono>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/bx.h>
#include <bx/mutex.h>
#include <bx/thread.h>
#include "views/mesh_view.h"
#include "glfw_app.h"

int renderThread(bx::Thread *thread, void* userData) {
  return 0;
}


class LabelStudio : public GLFWApp {
private:
  bool dragging = false;
  double mouseDownX, mouseDownY;
public:
  LabelStudio() : GLFWApp("LabelStudio") {
    auto mesh_view = std::make_shared<views::MeshView>("../bunny.ply");
    setView(mesh_view);
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
  }

  void scroll(double xoffset, double yoffset) {
    (void)xoffset;
    double diff = yoffset * 0.05;
    eyePos[2] = std::min(eyePos[2] + diff, -0.1);
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
