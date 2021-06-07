#include "label_studio.h"
#include <memory>
#include <filesystem>
#include <fstream>
#include <bgfx/bgfx.h>
#include "3rdparty/json.hpp"
#include "commands/keypoints.h"
#include "commands/bounding_box.h"
#include "id.h"

LabelStudio::LabelStudio(const std::string& folder) : GLFWApp("Studio"), viewId(IdFactory::getInstance().getId()),
                                                      sceneModel(folder), studioViewController(sceneModel, timeline, viewId), timeline(sceneModel), datasetFolder(folder) {
  loadState();

  glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    LabelStudio* w = (LabelStudio*)glfwGetWindowUserPointer(window);
    w->setInputModifier(mods);
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
      w->leftButtonDown(x, y);
    } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
      w->leftButtonUp(x, y);
    }
  });

  glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
    LabelStudio* w = (LabelStudio*)glfwGetWindowUserPointer(window);
    w->mouseMoved(xpos, ypos);
  });

  glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
    LabelStudio* w = (LabelStudio*)glfwGetWindowUserPointer(window);
    w->scroll(xoffset, yoffset);
  });
  glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height) {
    LabelStudio* w = (LabelStudio*)glfwGetWindowUserPointer(window);
    w->resize(width, height);
  });

  glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
    LabelStudio* w = (LabelStudio*)glfwGetWindowUserPointer(window);
    w->setInputModifier(mods);
    if (action == GLFW_PRESS) {
      if ((CommandModifier == mods) && (GLFW_KEY_S == key)) {
        w->sceneModel.save();
      } else if ((CommandModifier == mods) && (GLFW_KEY_Z == key)) {
        w->undo();
      } else {
        char characterPressed = key;
        w->studioViewController.keypress(characterPressed, w->inputModifier);
      }
    }
  });

  studioViewController.viewWillAppear(width, height);
  loadState();
}

void LabelStudio::setInputModifier(int mods) {
  if (mods == CommandModifier) {
    inputModifier = inputModifier | ModCommand; //Only set bits of inputModifier
  } else {
    inputModifier = ModNone;
  }
}

void LabelStudio::leftButtonDown(double x, double y) {
  studioViewController.leftButtonDown(x, y, inputModifier);
}

void LabelStudio::leftButtonUp(double x, double y) {
  studioViewController.leftButtonUp(x, y, inputModifier);
}

void LabelStudio::mouseMoved(double x, double y) {
  studioViewController.mouseMoved(x, y, inputModifier);
}

void LabelStudio::scroll(double xoffset, double yoffset) {
  studioViewController.scroll(xoffset, yoffset, inputModifier);
}

void LabelStudio::resize(int newWidth, int newHeight) {
  GLFWApp::resize(newWidth, newHeight);
  studioViewController.resize(newWidth, newHeight, inputModifier);
}

bool LabelStudio::update() const {
  bgfx::setViewRect(viewId, 0, 0, width, height);
  studioViewController.render();

  bgfx::frame();

  glfwWaitEventsTimeout(0.16);

  return !glfwWindowShouldClose(window);
}

void LabelStudio::undo() {
  timeline.undoCommand();
  studioViewController.refresh();
}

void LabelStudio::loadState() {
  std::filesystem::path annotationPath(datasetFolder / "annotations.json");
  if (!std::filesystem::exists(annotationPath))
    return;
  std::ifstream file(annotationPath.string());
  nlohmann::json json;
  file >> json;
  for (auto& keypoint : json["keypoints"]) {
    auto position = keypoint["position"];
    auto k = Vector3f(position[0].get<float>(), position[1].get<float>(), position[2].get<float>());
    Keypoint kp(-1, keypoint["instance_id"].get<int>(), k);
    std::unique_ptr<Command> command = std::make_unique<commands::AddKeypointCommand>(kp);
    timeline.pushCommand(std::move(command));
  }
  for (auto& bbox : json["bounding_boxes"]) {
    auto p = bbox["position"];
    auto orn = bbox["orientation"];
    auto d = bbox["dimensions"];
    BBox box = {
        .position = Vector3f(p[0].get<float>(), p[1].get<float>(), p[2].get<float>()),
        .orientation = Quaternionf(orn["w"].get<float>(), orn["x"].get<float>(), orn["y"].get<float>(), orn["z"].get<float>()),
        .dimensions = Vector3f(d[0].get<float>(), d[1].get<float>(), d[2].get<float>())};
    std::unique_ptr<Command> command = std::make_unique<commands::AddBBoxCommand>(box);
    timeline.pushCommand(std::move(command));
  }
}
