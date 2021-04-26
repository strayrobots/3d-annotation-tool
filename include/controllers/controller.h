#pragma once
#include "view_context_3d.h"

namespace controllers {
class Controller {
public:
  virtual void viewWillAppear(int width, int height) {};
  virtual void resize(int width, int height) {};

  virtual bool leftButtonDown(const ViewContext3D& viewContext) { return false; };
  virtual bool leftButtonUp(const ViewContext3D& viewContext) { return false; };
  virtual bool mouseMoved(const ViewContext3D& viewContext) { return false; };
  virtual bool scroll(double xoffset, double yoffset) { return false; };
  virtual bool keypress(char character) {return false; };
};
}
