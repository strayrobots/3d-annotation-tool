#include <GLFW/glfw3.h>
#include <bgfx/platform.h>
#include <bx/handlealloc.h>
#include <bx/thread.h>
#include <bx/mutex.h>
#include <iostream>
#if BX_PLATFORM_OSX
#define GLFW_EXPOSE_NATIVE_COCOA
#endif
#include <GLFW/glfw3native.h>

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
    std::cout << "Created window" << std::endl;

    init();
    glfwShowWindow(window);
    std::cout << "Init done." << std::endl;
  }

  ~GLFWWindow() {
    glfwDestroyWindowImpl(window);
    glfwTerminate();
    bgfx::shutdown();
  }

  bool update() {
    glfwWaitEventsTimeout(0.016);
    bgfx::setViewRect(0, 0, 0, uint16_t(800), uint16_t(600));
    bgfx::touch(0);
    bgfx::dbgTextClear();
    bgfx::frame();
    return !glfwWindowShouldClose(window);
  }

protected:
  void init() {
    bgfx::Init init;
#if BX_PLATFORM_OSX
    init.platformData.ndt = NULL;
#endif
    init.platformData.nwh = glfwNativeWindowHandle(window);

    init.resolution.width = 800;
    init.resolution.height = 600;
    init.resolution.reset = BGFX_RESET_VSYNC;

    bgfx::renderFrame();
    bgfx::init(init);

    bgfx::setDebug(BGFX_DEBUG_TEXT);

    // Set view 0 clear state.
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);
  }

};

