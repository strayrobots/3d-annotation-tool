#ifndef H_VIEW
#define H_VIEW
#include "camera.h"

namespace views {

using namespace Eigen;
class View {
public:
  View() {}
  virtual void render(const Camera& camera) const = 0;
};

class SizedView : public View {
protected:
  int width, height;
public:
  SizedView(int w, int h) : View(), width(w), height(h) {};
  void resize(int newWidth, int newHeight) {
    width = newWidth;
    height = newHeight;
  }
};

}

#endif

