#pragma once
#include "views/view.h"
#include <GLFW/glfw3.h>
#include <memory>

#if BX_PLATFORM_OSX
const unsigned int CommandModifier = GLFW_MOD_SUPER;
#else
const unsigned int CommandModifier = GLFW_MOD_CONTROL;
#endif

enum class InputModifier
{
  command,
  NONE
};

class GLFWApp
{
protected:
  GLFWwindow* window;
  std::shared_ptr<views::View> view;
  int width = 800;
  int height = 600;

public:
  GLFWApp(std::string name);
  ~GLFWApp();
  void resize(int newWidth, int newHeight);
  void setView(std::shared_ptr<views::View> v);
  virtual bool update() const = 0;
};
