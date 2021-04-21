#include <iostream>
#include "geometry/ray_trace_mesh.h"


namespace geometry {
RayTraceMesh::RayTraceMesh(const geometry::TriangleMesh& mesh) :
    nanoMesh(mesh.vertices().data(), mesh.faces().data(), sizeof(float) * 3) {
  nanort::TriangleSAHPred<float> trianglePred(mesh.vertices().data(), mesh.faces().data(), sizeof(float) * 3);
  nanort::BVHBuildOptions<float> build_options;
  std::cout << "Building bounding volume hierarchy.\r" << std::flush;
  auto ret = bvh.Build(mesh.faces().rows(), nanoMesh, trianglePred, build_options);
  assert(ret && "Can't build bounding volume hierarchy.");
  std::cout << "Done building bounding volume hierarchy." << std::endl;
  nanort::BVHBuildStatistics stats = bvh.GetStatistics();
}

std::optional<Vector3f> RayTraceMesh::traceRay(const Vector3f& origin, const Vector3f& direction) const {
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
    Vector3f point = (1.0f - isect.u - isect.v) * vertex1 + isect.u * vertex2 + isect.v * vertex3;
    return std::make_optional(point);
  }
  return {};
}
}
