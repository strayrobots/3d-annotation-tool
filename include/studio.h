#pragma once
#include <bgfx/platform.h>
#include "glfw_app.h"
#include "scene_model.h"
#include <memory>
#include <filesystem>
#include <fstream>
#include <bgfx/bgfx.h>
#include "3rdparty/json.hpp"
#include "commands/keypoints.h"
#include "commands/bounding_box.h"
#include "commands/rectangle.h"
#include "utils/serialize.h"
#include "timeline.h"

using namespace commands;
template <class ViewController>
class Studio : public GLFWApp {
public:
  ViewController viewController;
  InputModifier inputModifier = ModNone;

  Studio(const std::string& folder) : GLFWApp("Stray 3D Annotation Tool", 1200, 800), viewController(folder) {
    glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
      double x, y;
      glfwGetCursorPos(window, &x, &y);
      Studio* w = (Studio*)glfwGetWindowUserPointer(window);
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
      w->setInputModifier(key);
      if (action == GLFW_PRESS) {
        if ((CommandModifier == mods) && (GLFW_KEY_S == key)) {
          w->viewController.save();
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
    viewController.load();
  }

  void leftButtonDown(double x, double y) {
    viewController.leftButtonDown(x, y, inputModifier);
  }
  void setInputModifier(int key) {
    if (key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_LEFT_SUPER) {
      inputModifier = inputModifier ^ ModCommand;
    }
    if (key == GLFW_KEY_LEFT_SHIFT) {
      inputModifier = inputModifier ^ ModShift;
    }
    if (key == GLFW_KEY_LEFT_ALT) {
      inputModifier = inputModifier ^ ModAlt;
    }
    if (key == GLFW_KEY_LEFT_CONTROL) {
      inputModifier = inputModifier ^ ModCtrl;
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
    viewController.render(inputModifier);

    bgfx::frame();

    glfwWaitEventsTimeout(0.16);

    return !glfwWindowShouldClose(window);
  }

  void undo() {
    viewController.undo();
  }
};
