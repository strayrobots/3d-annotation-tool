#include <iostream>
#include "geometry/ray_trace_mesh.h"

namespace geometry {
RayTraceMesh::RayTraceMesh(std::shared_ptr<geometry::TriangleMesh> m) : nanoMesh(m->vertices().data(), m->faces().data(), sizeof(float) * 3) {
  mesh = m;
  nanort::TriangleSAHPred<float> trianglePred(mesh->vertices().data(), mesh->faces().data(), sizeof(float) * 3);
  nanort::BVHBuildOptions<float> build_options;
  auto ret = bvh.Build(mesh->faces().rows(), nanoMesh, trianglePred, build_options);
  assert(ret && "Can't build bounding volume hierarchy.");
  nanort::BVHBuildStatistics stats = bvh.GetStatistics();
}

Intersection RayTraceMesh::traceRayIntersection(const Vector3f& origin, const Vector3f& direction) const {
  Intersection intersection = { false, Vector3f::Zero(), Vector3f::Zero() };
  nanort::Ray<float> ray;
  ray.min_t = 0.0;
  ray.max_t = 1e9f;

  ray.org[0] = origin[0];
  ray.org[1] = origin[1];
  ray.org[2] = origin[2];

  ray.dir[0] = direction[0];
  ray.dir[1] = direction[1];
  ray.dir[2] = direction[2];
  const unsigned int* faces = nanoMesh.GetFaces();
  const float* vertices = nanoMesh.GetVertices();
  nanort::TriangleIntersector<float, nanort::TriangleIntersection<float>> triangleIntersector(vertices, faces, sizeof(float) * 3);
  nanort::TriangleIntersection<float> isect;
  bool pointingAtMesh = bvh.Traverse(ray, triangleIntersector, &isect);
  if (pointingAtMesh) {
    uint32_t faceId = isect.prim_id;
    const unsigned int* face = &faces[faceId * 3];
    const float* v1 = &vertices[face[0] * 3];
    const float* v2 = &vertices[face[1] * 3];
    const float* v3 = &vertices[face[2] * 3];
    Vector3f vertex1 = Vector3f(v1[0], v1[1], v1[2]);
    Vector3f vertex2 = Vector3f(v2[0], v2[1], v2[2]);
    Vector3f vertex3 = Vector3f(v3[0], v3[1], v3[2]);
    intersection.hit = true;
    intersection.point = (1.0f - isect.u - isect.v) * vertex1 + isect.u * vertex2 + isect.v * vertex3;
    const RowMatrixf& normals = mesh->getVertexNormals();
    const auto n1 = normals.row(face[0]);
    const auto n2 = normals.row(face[1]);
    const auto n3 = normals.row(face[2]);
    intersection.normal = (n1 + n2 + n3) / 3.0;
  }
  return intersection;
}

std::optional<Vector3f> RayTraceMesh::traceRay(const Vector3f& origin, const Vector3f& direction) const {
  Intersection its = traceRayIntersection(origin, direction);
  if (its.hit) {
    return its.point;
  }
  return {};
}
} // namespace geometry
