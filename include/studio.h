#pragma once
#include <bgfx/platform.h>
#include "glfw_app.h"
#include "scene_model.h"
#include "timeline.h"

#include <memory>
#include <filesystem>
#include <fstream>
#include <bgfx/bgfx.h>
#include "3rdparty/json.hpp"
#include "commands/keypoints.h"
#include "commands/bounding_box.h"
#include "commands/rectangle.h"
#include "utils/serialize.h"

using namespace commands;
template <class ViewController>
class Studio : public GLFWApp {
public:
  SceneModel sceneModel;
  ViewController viewController;
  InputModifier inputModifier = ModNone;

  Studio(const std::string& folder) : GLFWApp("Studio", 1200, 800),
                                      sceneModel(folder), viewController(sceneModel, folder) {
    glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
      double x, y;
      glfwGetCursorPos(window, &x, &y);
      Studio* w = (Studio*)glfwGetWindowUserPointer(window);
      w->setInputModifier(mods);
      if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        w->leftButtonDown(x, y);
      } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        w->leftButtonUp(x, y);
      }
    });

    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
      Studio* w = (Studio*)glfwGetWindowUserPointer(window);
      w->mouseMoved(xpos, ypos);
    });

    glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
      Studio* w = (Studio*)glfwGetWindowUserPointer(window);
      w->scroll(xoffset, yoffset);
    });
    glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height) {
      Studio* w = (Studio*)glfwGetWindowUserPointer(window);
      w->resize(width, height);
    });

    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
      Studio* w = (Studio*)glfwGetWindowUserPointer(window);
      w->setInputModifier(mods);
      if (action == GLFW_PRESS) {
        if ((CommandModifier == mods) && (GLFW_KEY_S == key)) {
          w->sceneModel.save();
        } else if ((CommandModifier == mods) && (GLFW_KEY_Z == key)) {
          w->undo();
        } else {
          char characterPressed = key;
          w->viewController.keypress(characterPressed, w->inputModifier);
        }
      }
    });

    views::Rect rect = {0.0f, 0.0f, float(width), float(height)};
    viewController.viewWillAppear(rect);
    viewController.loadState();
  }

  void leftButtonDown(double x, double y) {
    viewController.leftButtonDown(x, y, inputModifier);
  }
  void setInputModifier(int mods) {
    inputModifier = ModNone;
    if (mods & CommandModifier) {
      inputModifier = inputModifier | ModCommand; // Only set bits of inputModifier
    }
    if (mods & ShiftModifier) {
      inputModifier = inputModifier | ModShift;
    }
    if (mods & AltModifier) {
      inputModifier = inputModifier | ModAlt;
    }
    if (mods & CtrlModifier) {
      inputModifier = inputModifier | ModCtrl;
    }
  }
  void leftButtonUp(double x, double y) {
    viewController.leftButtonUp(x, y, inputModifier);
  }
  void mouseMoved(double x, double y) {
    viewController.mouseMoved(x, y, inputModifier);
  }
  void scroll(double xoffset, double yoffset) {
    viewController.scroll(xoffset, yoffset, inputModifier);
  }
  void resize(int newWidth, int newHeight) {
    GLFWApp::resize(newWidth, newHeight);
    views::Rect rect = {0.0f, 0.0f, float(newWidth), float(newHeight)};
    viewController.resize(rect);
  }
  bool update() const {
    viewController.render();

    bgfx::frame();

    glfwWaitEventsTimeout(0.16);

    return !glfwWindowShouldClose(window);
  }

  void undo() {
    viewController.timeline.undoCommand();
    viewController.refresh();
  }
};

/*
protected:
  Timeline timeline;
  const std::filesystem::path datasetFolder;

  void loadState() {
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
      auto instanceId = bbox["instance_id"];
      BBox box = {
          .instanceId = instanceId,
          .position = utils::serialize::toVector3(p),
          .orientation = Quaternionf(orn["w"].get<float>(), orn["x"].get<float>(), orn["y"].get<float>(), orn["z"].get<float>()),
          .dimensions = utils::serialize::toVector3(d),
      };
      std::unique_ptr<Command> command = std::make_unique<commands::AddBBoxCommand>(box);
      timeline.pushCommand(std::move(command));
    }
    for (auto& rectangle : json["rectangles"]) {
      Rectangle rect(0, rectangle["class_id"],
                     utils::serialize::toVector3(rectangle["center"]),
                     utils::serialize::toQuaternion(rectangle["orientation"]),
                     utils::serialize::toVector2(rectangle["size"]));
      std::unique_ptr<Command> command = std::make_unique<commands::AddRectangleCommand>(rect);
      timeline.pushCommand(std::move(command));
    }
  }
};

*/