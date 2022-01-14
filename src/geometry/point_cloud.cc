#include "3rdparty/happly.h"
#include "geometry/point_cloud.h"

namespace geometry {
PointCloud::PointCloud(const std::string& filepath) {
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
}
