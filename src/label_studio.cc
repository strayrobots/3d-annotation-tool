#include "label_studio.h"
#include <memory>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <bgfx/bgfx.h>
#include "3rdparty/json.hpp"

LabelStudio::LabelStudio(const std::string& folder) : GLFWApp("Label Studio"), sceneModel(folder),
  studioViewController(sceneModel), datasetFolder(folder)
{
  loadState();

  glfwSetMouseButtonCallback(window, [](GLFWwindow *window, int button, int action, int mods) {
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    LabelStudio* w = (LabelStudio*)glfwGetWindowUserPointer(window);
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
      w->leftButtonDown(x, y);
    } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
      w->leftButtonUp(x, y);
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

  glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
    LabelStudio* w = (LabelStudio*)glfwGetWindowUserPointer(window);
    if (action == GLFW_PRESS) {
      if((CommandModifier == mods) && (GLFW_KEY_S == key)) {
        w->sceneModel.save();
      } else if ((CommandModifier == mods) && (GLFW_KEY_Z == key)) {
        w->studioViewController.undo();
      } else {
        char characterPressed = key;
        w->studioViewController.keypress(characterPressed);
      }
    }
  });
}

void LabelStudio::leftButtonDown(double x, double y) {
  studioViewController.leftButtonDown(x, y);
}

void LabelStudio::leftButtonUp(double x, double y) {
  studioViewController.leftButtonUp(x, y);
}

void LabelStudio::mouseMoved(double x, double y) {
  studioViewController.mouseMoved(x, y);
}

void LabelStudio::scroll(double xoffset, double yoffset) {
  studioViewController.scroll(xoffset, yoffset);
}

bool LabelStudio::update() const {
  bgfx::setViewRect(0, 0, 0, uint16_t(Width), uint16_t(Height));
  glfwWaitEventsTimeout(0.02);
  studioViewController.render();

  bgfx::frame();

  return !glfwWindowShouldClose(window);
}

void LabelStudio::loadState() {
  std::filesystem::path keypointPath(datasetFolder / "keypoints.json");
  if (!std::filesystem::exists(keypointPath)) return;
  std::ifstream file(keypointPath.string());
  nlohmann::json json;
  file >> json;
  for (auto& keypoint : json) {
    auto k = Vector3f(keypoint["x"].get<float>(), keypoint["y"].get<float>(), keypoint["z"].get<float>());
    std::unique_ptr<Command> command = std::make_unique<AddKeypointCommand>(k);
    command->execute(studioViewController, sceneModel);
    studioViewController.pushCommand(std::move(command));
  }
}

