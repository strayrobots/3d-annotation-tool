#pragma once
#include <eigen3/Eigen/Core>

namespace geometry {

using RowMatrixf = Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor>;
using RowMatrixu8 = Eigen::Matrix<uint32_t, Eigen::Dynamic, 3, Eigen::RowMajor>;

class PointCloud {
public:
  RowMatrixf points;
  RowMatrixu8 colors;
  PointCloud(const std::string& filepath);
  Eigen::RowVector3f getMean() const;
  Eigen::RowVector3f getStd() const;
};
} // namespace geometry
