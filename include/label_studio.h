#ifndef H_LABEL_STUDIO
#define H_LABEL_STUDIO
#include <memory>
#include <thread>
#include <chrono>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <iostream>
#include <filesystem>
#include <fstream>
#include "3rdparty/json.hpp"
#include "3rdparty/cxxopts.h"
#include "glfw_app.h"
#include "scene_model.h"
#include "controllers/studio_view_controller.h"
#include "commands/command.h"
#include "commands/keypoints.h"

#if BX_PLATFORM_OSX
const unsigned int CommandModifier = GLFW_MOD_SUPER;
#else
const unsigned int CommandModifier = GLFW_MOD_CONTROL;
#endif

using namespace commands;
class LabelStudio : public GLFWApp {
private:
  const std::filesystem::path datasetFolder;
public:
  SceneModel sceneModel;
  StudioViewController studioViewController;

  LabelStudio(const std::string& folder) : GLFWApp("Label Studio"), sceneModel(folder),
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
          LabelStudio* w = (LabelStudio*)glfwGetWindowUserPointer(window);
          w->sceneModel.save();
        } else if ((CommandModifier == mods) && (GLFW_KEY_Z == key)) {
          w->studioViewController.undo();
        }
      }
    });
  }

  void leftButtonDown(double x, double y) {
    studioViewController.leftButtonDown(x, y);
  }

  void leftButtonUp(double x, double y) {
    studioViewController.leftButtonUp(x, y);
  }

  void mouseMoved(double x, double y) {
    studioViewController.mouseMoved(x, y);
  }

  void scroll(double xoffset, double yoffset) {
    (void)xoffset;
    double diff = yoffset * 0.05;
    const auto& cameraPosition = sceneModel.getCamera().getPosition();
    double newNorm = std::max(cameraPosition.norm() + diff, 0.1);
    sceneModel.setCameraPosition(newNorm * cameraPosition.normalized());
  }

  bool update() const override {
    bgfx::setViewRect(0, 0, 0, uint16_t(Width), uint16_t(Height));
    glfwWaitEventsTimeout(0.02);
    studioViewController.render(sceneModel.getCamera());

    bgfx::frame();

    return !glfwWindowShouldClose(window);
  }
protected:
  void loadState() {
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
};
#endif
