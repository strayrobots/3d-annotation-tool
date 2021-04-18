#ifndef H_LABEL_STUDIO
#define H_LABEL_STUDIO
#include <memory>
#include <thread>
#include <chrono>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <iostream>
#include <filesystem>
#include <list>
#include <fstream>
#include "3rdparty/json.hpp"
#include "3rdparty/cxxopts.h"
#include "glfw_app.h"
#include "scene_model.h"
#include "studio_view.h"
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
  std::list<std::unique_ptr<Command>> commandStack;

  // Changing view point.
  double prevX, prevY;
  bool dragging = false, moved = false;
  std::optional<Vector3f> pointingAt;
  const std::filesystem::path datasetFolder;
public:
  SceneModel sceneModel;
  StudioView studioView;

  LabelStudio(const std::string& folder) : GLFWApp("LabelStudio"), sceneModel(folder),
    studioView(sceneModel), datasetFolder(folder)
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
          w->undo();
        }
      }
    });
  }

  void leftButtonDown(double x, double y) {
    dragging = true;
    moved = false;
    prevX = x;
    prevY = y;
  }

  void leftButtonUp(double x, double y) {
    dragging = false;
    if (!moved && pointingAt.has_value()) {
      std::unique_ptr<Command> command = std::make_unique<AddKeypointCommand>(pointingAt.value());
      command->execute(studioView, sceneModel);
      commandStack.push_back(std::move(command));
      std::cout << "Added keypoint: " << pointingAt.value().transpose() << std::endl;
    }
  }

  void mouseMoved(double x, double y) {
    moved = true;
    
    if (dragging) {
      float diffX = (x - prevX);
      float diffY = (y - prevY);
      Quaternionf q = AngleAxisf( diffX*M_PI/2000, Vector3f::UnitY())
                            * AngleAxisf(diffY*M_PI/2000, Vector3f::UnitX());
      sceneModel.rotateCamera(q);

      prevX = x;
      prevY = y;
      
    }
    pointingAt = sceneModel.traceRay(x, y);
  
    
  }

  void scroll(double xoffset, double yoffset) {
    (void)xoffset;
    float diff = yoffset * 0.05;
    sceneModel.zoomCamera(diff);
  }

  void undo() {
    if (commandStack.empty()) return;
    commandStack.back()->undo(studioView, sceneModel);
    commandStack.pop_back();
  }

  bool update() const override {
    bgfx::setViewRect(0, 0, 0, uint16_t(Width), uint16_t(Height));
    glfwWaitEventsTimeout(0.02);
    studioView.render(sceneModel.getCamera());

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
      command->execute(studioView, sceneModel);
      commandStack.push_back(std::move(command));
    }
  }
};
#endif
