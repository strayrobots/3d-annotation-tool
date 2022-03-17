#include "geometry/ray_trace_cloud.h"

using namespace geometry;
using namespace particle_tracing;

using RowMatrixf = Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor>;

RayTraceCloud::RayTraceCloud(std::shared_ptr<geometry::PointCloud> pc, float& size) : pointCloud(pc), pointSize(size) {
  nanort::BVHBuildOptions<float> options;
  options.cache_bbox = false;
  RowMatrixf vertices = pointCloud->points;
  SphereGeometry sphereGeometry(vertices.data(), 0.01);
  SpherePred spherePredicate(vertices.data());
  assert(bvh.Build(vertices.rows(), sphereGeometry, spherePredicate, options));
}

Intersection RayTraceCloud::traceRayIntersection(const Vector3f& origin, const Vector3f& direction) const {
  if (pointCloud == nullptr) return {false, Vector3f::Zero(), Vector3f::Zero()};
  nanort::Ray<float> ray;
  ray.min_t = 0.0;
  ray.max_t = 1e9f;
  ray.org[0] = origin[0];
  ray.org[1] = origin[1];
  ray.org[2] = origin[2];

  ray.dir[0] = direction[0];
  ray.dir[1] = direction[1];
  ray.dir[2] = direction[2];

  float pointRadius = 0.005f * pointSize;
  const float* points = &pointCloud->points.data()[0];
  SphereIntersector<SphereIntersection> intersector(points, pointRadius);
  SphereIntersection intersection;
  bool hit = bvh.Traverse(ray, intersector, &intersection);
  if (hit) {
    unsigned int pointId = intersection.prim_id;
    Vector3f position = pointCloud->points.row(pointId).transpose();
    Vector3f hitPoint = origin + intersection.t * direction;
    Vector3f normal = (hitPoint - position).normalized();
    return {true, position, normal};
  } else {
    return {false, Vector3f::Zero(), Vector3f::Zero()};
  }
}

std::optional<Vector3f> RayTraceCloud::traceRay(const Vector3f& origin, const Vector3f& direction) const {
  Intersection its = traceRayIntersection(origin, direction);
  if (its.hit) {
    return its.point;
  }
  return {};
}
