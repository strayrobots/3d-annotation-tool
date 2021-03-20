#include <thread>
#include <chrono>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/bx.h>
#include <bx/mutex.h>
#include <bx/thread.h>
#include "window/glfw_window.h"

int main(void) {
  GLFWWindow window("LabelStudio");
  while (window.update()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
  }
}
