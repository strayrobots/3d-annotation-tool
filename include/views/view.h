#ifndef H_VIEW
#define H_VIEW
#include "camera.h"

namespace views {

using namespace Eigen;
class View {
protected:
  int width, height;

public:
  View(int w, int h) : width(w), height(h) {}
  virtual void render(const Camera& camera) const = 0;
  void resize(int newWidth, int newHeight) {
    width = newWidth;
    height = newHeight;
  }
};

} // namespace views

#endif
