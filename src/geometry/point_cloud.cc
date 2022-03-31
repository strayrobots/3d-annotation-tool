#include "3rdparty/happly.h"
#include "geometry/point_cloud.h"

namespace geometry {
PointCloud::PointCloud(const std::string& filepath) {
  happly::PLYData plyIn(filepath);
  const auto& vertices = plyIn.getVertexPositions();
  const auto& vertexColors = plyIn.getVertexColors();
  points.resize(vertices.size(), 3);
  colors.resize(vertexColors.size(), 3);

  for (unsigned int i = 0; i < vertices.size(); i++) {
    for (uint8_t j = 0; j < 3; j++) {
      points(i, j) = vertices[i][j];
      colors(i, j) = vertexColors[i][j];
    }
  }
}

Eigen::RowVector3f PointCloud::getMean() const {
  Eigen::RowVector3f mean = points.colwise().mean();
  return mean;
}

Eigen::RowVector3f PointCloud::getStd() const {
  Eigen::VectorXf x = points.col(0);
  Eigen::VectorXf y = points.col(1);
  Eigen::VectorXf z = points.col(2);

  float std_x = std::sqrt(((x - x.mean() * Eigen::VectorXf::Ones(x.size())).array().square().sum() / (x.size() - 1)));
  float std_y = std::sqrt(((y - y.mean() * Eigen::VectorXf::Ones(y.size())).array().square().sum() / (y.size() - 1)));
  float std_z = std::sqrt(((z - z.mean() * Eigen::VectorXf::Ones(z.size())).array().square().sum() / (z.size() - 1)));

  Eigen::RowVector3f std(std_x, std_y, std_z);
  return std;
}

} // namespace geometry
