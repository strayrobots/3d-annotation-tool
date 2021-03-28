#ifndef H_VIEW
#define H_VIEW
#include <eigen3/Eigen/Dense>

namespace views {

class View {
public:
  virtual void render(const Eigen::Vector3f& eye) = 0;
};

}

#endif

