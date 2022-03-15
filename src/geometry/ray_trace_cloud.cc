#include "geometry/ray_trace_cloud.h"

using namespace geometry;

using RowMatrixf = Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor>;

RayTraceCloud::RayTraceCloud(std::shared_ptr<geometry::PointCloud> pc) : pointCloud(pc) {
  nanort::BVHBuildOptions<float> options;
  options.cache_bbox = false;
  RowMatrixf vertices = pointCloud->points;
  particle_tracing::SphereGeometry sphereGeometry(vertices.data(), 0.01);
  particle_tracing::SpherePred spherePredicate(vertices.data());
  assert(bvh.Build(vertices.rows(), sphereGeometry, spherePredicate, options));
}

std::optional<Vector3f> RayTraceCloud::traceRay(const Vector3f& origin, const Vector3f& direction) const {
  return Vector3f(0.0f, 0.0f, 0.0f);
}

Intersection RayTraceCloud::traceRayIntersection(const Vector3f& origin, const Vector3f& direction) const {
  return {false, Vector3f::Zero(), Vector3f::Zero()};
}
