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

class GLFWApp {
protected:
  GLFWwindow* window;
  std::shared_ptr<views::View> view;
  double width = 800;
  double height = 600;
  Eigen::Matrix3f currentRotation = Eigen::Matrix3f::Identity();
  Eigen::Matrix3f rotationStart = Eigen::Matrix3f::Identity();
  Eigen::Vector3f eyePos = Eigen::Vector3f(0.0, 0.0, -1.0);
public:
  GLFWApp(std::string name) {
    glfwSetErrorCallback(errorCb);
    if (!glfwInit()) {
      std::cout << "Could not initialize glfw" << std::endl;
      return;
    }
#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
#else
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#endif
    window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
    if (!window) {
      std::cout << "Failed to create window." << std::endl;
    }

    glfwSetWindowUserPointer(window, this);

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

  ~GLFWApp() {
    view = nullptr;
    glfwDestroyWindowImpl(window);
    glfwTerminate();
    bgfx::shutdown();
  }

  void setView(std::shared_ptr<views::View> v) { view = v; }

  bool update() {
    bgfx::setDebug(BGFX_DEBUG_TEXT);

    glfwWaitEventsTimeout(0.016);
    bgfx::setViewRect(0, 0, 0, uint16_t(width), uint16_t(height));
    view->render(eyePos, currentRotation);

    bgfx::frame();

    return !glfwWindowShouldClose(window);
  }

};

