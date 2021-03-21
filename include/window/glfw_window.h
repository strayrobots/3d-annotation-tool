#include <memory>
#include <GLFW/glfw3.h>
#include <bgfx/platform.h>
#include <bx/handlealloc.h>
#include <bx/thread.h>
#include <bx/mutex.h>
#include <iostream>
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Geometry>
#if BX_PLATFORM_OSX
#define GLFW_EXPOSE_NATIVE_COCOA
#endif
#include <GLFW/glfw3native.h>

#include "views/view.h"
using namespace Eigen;

static void* glfwNativeWindowHandle(GLFWwindow* _window) {
# if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
  return (void*)(uintptr_t)glfwGetX11Window(_window);
# elif BX_PLATFORM_OSX
  return glfwGetCocoaWindow(_window);
# elif BX_PLATFORM_WINDOWS
  return glfwGetWin32Window(_window);
# endif // BX_PLATFORM_
}

static void glfwDestroyWindowImpl(GLFWwindow *_window) {
  if(!_window)
    return;
# if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
#   if ENTRY_CONFIG_USE_WAYLAND
  wl_egl_window *win_impl = (wl_egl_window*)glfwGetWindowUserPointer(_window);
  if (win_impl) {
    glfwSetWindowUserPointer(_window, nullptr);
    wl_egl_window_destroy(win_impl);
  }
#		endif
#	endif
  glfwDestroyWindow(_window);
}

static void errorCb(int _error, const char* _description) {
  std::cout << "GLFW error " << _error << ":" <<  _description << std::endl;
}

class GLFWWindow {
private:
  GLFWwindow* window;
  std::shared_ptr<views::View> view;
  bool dragging = false;
  double mouseDownX, mouseDownY;
  double width = 800;
  double height = 600;
  Eigen::Matrix3f currentRotation = Eigen::Matrix3f::Identity();
  Eigen::Matrix3f rotationStart = Eigen::Matrix3f::Identity();
  Eigen::Vector3f eyePos = Eigen::Vector3f(0.0, 0.0, -1.0);
public:
  GLFWWindow(std::string name) {
    glfwSetErrorCallback(errorCb);
    if (!glfwInit()) {
      std::cout << "Could not initialize glfw" << std::endl;
      return;
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(800, 600, name.c_str(), NULL, NULL);
    if (!window) {
      std::cout << "Failed to create window." << std::endl;
    }
    glfwSetWindowUserPointer(window, this);
    glfwSetMouseButtonCallback(window, [](GLFWwindow *window, int button, int action, int mods) {
      GLFWWindow* w = (GLFWWindow*)glfwGetWindowUserPointer(window);
      if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        w->leftButtonDown();
      } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        w->leftButtonUp();
      }
    });
    glfwSetCursorPosCallback(window, [](GLFWwindow *window, double xpos, double ypos) {
      GLFWWindow* w = (GLFWWindow*)glfwGetWindowUserPointer(window);
      w->mouseMoved(xpos, ypos);
    });

    glfwSetScrollCallback(window, [](GLFWwindow *window, double xoffset, double yoffset) {
      GLFWWindow* w = (GLFWWindow*)glfwGetWindowUserPointer(window);
      w->scroll(xoffset, yoffset);
    });

    bgfx::renderFrame();

    bgfx::Init init;
#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
    init.platformData.ndt = glfwGetX11Display();
#elif BX_PLATFORM_OSX
    init.platformData.ndt = NULL;
#endif
    init.platformData.nwh = glfwNativeWindowHandle(window);
    init.platformData.context = nullptr;
    init.platformData.backBuffer = nullptr;
    init.platformData.backBufferDS = nullptr;

    int width, height;
    glfwGetWindowSize(window, &width, &height);
    init.resolution.width = (uint32_t)width;
    init.resolution.height = (uint32_t)height;
    init.resolution.reset = BGFX_RESET_VSYNC;

    if (!bgfx::init(init)) {
      std::cout << "Could not init bgfx!" << std::endl;
    }
    // Set view 0 clear state.
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);
    bgfx::setViewRect(0, 0, 0, bgfx::BackbufferRatio::Equal);

    glfwShowWindow(window);
  }

  ~GLFWWindow() {
    view = nullptr;
    glfwDestroyWindowImpl(window);
    glfwTerminate();
    bgfx::shutdown();
  }

  void setView(std::shared_ptr<views::View> v) { view = v; }

  bool update() {
    bgfx::setDebug(BGFX_DEBUG_TEXT);

    glfwWaitEventsTimeout(0.016);
    bgfx::setViewRect(0, 0, 0, uint16_t(800), uint16_t(600));
    view->render(eyePos, currentRotation);

    bgfx::frame();

    return !glfwWindowShouldClose(window);
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

