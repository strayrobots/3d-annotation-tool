#ifndef H_VIEW
#define H_VIEW
#include <eigen3/Eigen/Dense>

namespace views {

class View {
public:
  virtual void render(const Eigen::Vector3f& eye, const Eigen::Matrix3f& rotation) = 0;
};

}

#endif

