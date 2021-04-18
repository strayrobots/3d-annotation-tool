#ifndef H_VIEW
#define H_VIEW
#include "camera.h"

namespace views {

using namespace Eigen;
class View {
public:
  virtual void render(std::shared_ptr<Camera> camera) const = 0;
};

}

#endif

