#include "glfw_app.h"
#include <bgfx/platform.h>
#include <bx/handlealloc.h>
#include <bx/thread.h>
#include <bx/mutex.h>
#include <iostream>
#include <eigen3/Eigen/Dense>
#if BX_PLATFORM_OSX
#define GLFW_EXPOSE_NATIVE_COCOA
#endif
#if BX_PLATFORM_LINUX
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
#endif
#include <GLFW/glfw3native.h>
using namespace Eigen;

static void* glfwNativeWindowHandle(GLFWwindow* _window) {
#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
  return (void*)(uintptr_t)glfwGetX11Window(_window);
#elif BX_PLATFORM_OSX
  return glfwGetCocoaWindow(_window);
#elif BX_PLATFORM_WINDOWS
  return glfwGetWin32Window(_window);
#endif // BX_PLATFORM_
}

static void glfwDestroyWindowImpl(GLFWwindow* _window) {
  if (!_window)
    return;
#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
#if ENTRY_CONFIG_USE_WAYLAND
  wl_egl_window* win_impl = (wl_egl_window*)glfwGetWindowUserPointer(_window);
  if (win_impl) {
    glfwSetWindowUserPointer(_window, nullptr);
    wl_egl_window_destroy(win_impl);
  }
#endif
#endif
  glfwDestroyWindow(_window);
}

static void errorCb(int _error, const char* _description) {
  std::cout << "GLFW error " << _error << ":" << _description << std::endl;
}

GLFWApp::GLFWApp(std::string name, int w, int h) : width(w), height(h) {
  glfwSetErrorCallback(errorCb);
  if (!glfwInit()) {
    std::cout << "Could not initialize glfw" << std::endl;
    return;
  }
#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
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
  init.resolution.reset = BGFX_RESET_VSYNC | BGFX_RESET_HIDPI;

  if (!bgfx::init(init)) {
    std::cout << "Could not init bgfx!" << std::endl;
  }
  glfwShowWindow(window);
  glfwGetWindowSize(window, &width, &height);
}

GLFWApp::~GLFWApp() {
  view = nullptr;
  glfwDestroyWindowImpl(window);
  glfwTerminate();
  bgfx::shutdown();
}

void GLFWApp::setView(std::shared_ptr<views::View> v) { view = v; }

void GLFWApp::resize(int newWidth, int newHeight) {
  width = newWidth;
  height = newHeight;
  bgfx::reset(width, height, BGFX_RESET_NONE | BGFX_RESET_VSYNC | BGFX_RESET_HIDPI);
}
