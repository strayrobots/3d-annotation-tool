#include <memory>
#include <thread>
#include <chrono>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/bx.h>
#include <bx/mutex.h>
#include <bx/thread.h>
#include "window/glfw_window.h"
#include "views/mesh_view.h"

int renderThread(bx::Thread *thread, void* userData) {
  return 0;
}

int main(void) {
  auto window = std::make_shared<GLFWWindow>("LabelStudio");

  auto mesh_view = std::make_shared<views::MeshView>("../bunny.ply");
  window->setView(mesh_view);

  while (window->update()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
  }

  mesh_view = nullptr;
  window = nullptr;

  return 0;
}
