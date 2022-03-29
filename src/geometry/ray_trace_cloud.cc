#include "geometry/ray_trace_cloud.h"
#include <iostream>

using namespace geometry;
using namespace particle_tracing;

const uint32_t FindClosest = 50;
uint32_t closestIndices[FindClosest];
float distances[FindClosest];

RayTraceCloud::RayTraceCloud(std::shared_ptr<geometry::PointCloud> pc, float& size) : pointCloud(pc), pointSize(size),
    adaptor(pointCloud->points),
    index(3, adaptor, {10}) {
  nanort::BVHBuildOptions<float> options;
  options.cache_bbox = false;
  RowMatrixf vertices = pointCloud->points;
  SphereGeometry sphereGeometry(vertices.data(), 0.01);
  SpherePred spherePredicate(vertices.data());
  bool ret = bvh.Build(vertices.rows(), sphereGeometry, spherePredicate, options);
  if (!ret) {
    std::cout << "Failed to initialize bounding volume hierarchy" << std::endl;
    exit(1);
  }
  index.buildIndex();
}

Vector3f estimateNormal(const RowVector3f& queryPoint, const RowMatrixf& points, uint32_t* indices) {
  Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor> ps(FindClosest, 3);
  for (uint32_t i=0; i < FindClosest; i++) {
    ps.row(i) = points.row(indices[i]);
  }

  RowVector3f centroid = ps.rowwise().mean();

  for (uint32_t i=0; i < FindClosest; i++) {
    ps.row(i) = ps.row(i) - centroid;
  }

  auto svd = ps.transpose().jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV);
  Vector3f normal = svd.matrixU().col(2);
  return normal.normalized();
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
    RowVector3f position = pointCloud->points.row(pointId);

    nanoflann::KNNResultSet<float, uint32_t, uint32_t> resultSet(FindClosest);
    resultSet.init(&closestIndices[0], &distances[0]);
    float queryPoint[3] = { position[0], position[1], position[2] };
    index.findNeighbors(resultSet, &queryPoint[0], nanoflann::SearchParams(10));
    Vector3f normal = estimateNormal(position, pointCloud->points, &closestIndices[0]).normalized();

    // If the normal is pointing in the same direction as the ray,
    // the normal is on the wrong side and we should flip it.
    if (direction.dot(normal) < 0.0) {
      normal = -normal;
    }

    return {true, position, normal};
  } else {
    return {false, Vector3f::Zero(), Vector3f::Zero()};
  }
}

std::optional<Vector3f> RayTraceCloud::traceRay(const Vector3f& origin, const Vector3f& direction) const {
  if (pointCloud == nullptr) return {};
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
    RowVector3f position = pointCloud->points.row(pointId);
    return position.transpose();
  }
  return {};
}
