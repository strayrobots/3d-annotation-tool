#include <memory>
#include <GLFW/glfw3.h>
#include "views/view.h"

class GLFWApp {
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
