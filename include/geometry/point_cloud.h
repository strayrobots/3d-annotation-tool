#pragma once
#include "3rdparty/happly.h"

namespace geometry {

using RowMatrixf = Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor>;
using RowMatrixu8 = Eigen::Matrix<uint32_t, Eigen::Dynamic, 3, Eigen::RowMajor>;

class PointCloud {
public:
  RowMatrixf points;
  RowMatrixu8 colors;

  PointCloud(const std::string& filepath) {
    happly::PLYData plyIn(filepath);
    const auto& vertices = plyIn.getVertexPositions();
    const auto& vertexColors = plyIn.getVertexColors();
    points.resize(vertices.size(), 3);
    colors.resize(vertexColors.size(), 3);

    for (unsigned int i=0; i < vertices.size(); i++) {
      for (uint8_t j=0; j < 3; j++) {
        points(i, j) = vertices[i][j];
        colors(i, j) = vertexColors[i][j];
      }
    }
  }
};
}
