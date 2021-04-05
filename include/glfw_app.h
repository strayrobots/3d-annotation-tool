#include <memory>
#include <GLFW/glfw3.h>
#include "views/view.h"

class GLFWApp {
protected:
  GLFWwindow* window;
  std::shared_ptr<views::View> view;
  double width = 800;
  double height = 600;
public:
  GLFWApp(std::string name);
  ~GLFWApp();
  void setView(std::shared_ptr<views::View> v);
  virtual bool update() const = 0;
};

