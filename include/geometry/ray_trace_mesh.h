#pragma once
#include <optional>
#include <memory>
#include "geometry/mesh.h"
#define NANORT_ENABLE_PARALLEL_BUILD 1
#include "3rdparty/nanort.h"

using namespace Eigen;
namespace geometry {

struct Intersection {
  bool hit;
  Vector3f point;
  Vector3f normal;
};

class RayTraceMesh {
private:
  // Geometry.
  std::shared_ptr<geometry::TriangleMesh> mesh;
  nanort::TriangleMesh<float> nanoMesh;
  nanort::BVHAccel<float> bvh;

public:
  RayTraceMesh(std::shared_ptr<geometry::TriangleMesh> mesh);
  std::optional<Vector3f> traceRay(const Vector3f& origin, const Vector3f& direction) const;
  Intersection traceRayIntersection(const Vector3f& origin, const Vector3f& direction) const;
};
} // namespace geometry
