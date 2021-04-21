#pragma once
#include <optional>
#include "geometry/mesh.h"
#define NANORT_ENABLE_PARALLEL_BUILD 1
#include "3rdparty/nanort.h"

using namespace Eigen;
namespace geometry {
class RayTraceMesh {
private:
  // Geometry.
  nanort::TriangleMesh<float> nanoMesh;
  nanort::BVHAccel<float> bvh;

public:
  RayTraceMesh(const geometry::TriangleMesh& mesh);
  std::optional<Vector3f> traceRay(const Vector3f& origin, const Vector3f& direction) const;
};
}
