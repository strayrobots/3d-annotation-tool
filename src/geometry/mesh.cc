
#include <3rdparty/happly.h>
#include <eigen3/Eigen/Dense>
#include <omp.h>
#include "views/view.h"
#include "shader_utils.h"
#include "views/mesh_view.h"

namespace geometry {

TriangleMesh::TriangleMesh(const Matrix4f T) : transform(T) {}

const Matrix4f& TriangleMesh::getTransform() const { return transform; }

void TriangleMesh::setTranslation(const Vector3f& t) {
  transform.block<3, 1>(0, 3) = t;
}

void TriangleMesh::setRotation(const Matrix3f& rotation) {
  transform.block<3, 3>(0, 0) = rotation;
}

void TriangleMesh::setTransform(const Matrix4f& T) {
  transform = T;
}

void TriangleMesh::computeNormals() {
  RowMatrixf faceNormals = RowMatrixf::Zero(F.rows(), 3);
#pragma omp parallel for
  for (int i = 0; i < F.rows(); i++) {
    auto vertex_indices = F.row(i);
    auto vertex1 = V.row(vertex_indices[0]);
    auto vertex2 = V.row(vertex_indices[1]);
    auto vertex3 = V.row(vertex_indices[2]);

    Eigen::RowVector3f a = (vertex1 - vertex2).normalized();
    Eigen::RowVector3f b = (vertex3 - vertex2).normalized();
    faceNormals.row(i) = a.cross(b);
  }
  vertexNormals.resize(V.rows(), 3);
  for (int i = 0; i < F.rows(); i++) {
    auto vertex_indices = F.row(i);
    Eigen::RowVector3f normal = faceNormals.row(i);
    vertexNormals.row(vertex_indices[0]) += normal;
    vertexNormals.row(vertex_indices[1]) += normal;
    vertexNormals.row(vertex_indices[2]) += normal;
  }

#pragma omp parallel for
  for (int i = 0; i < vertexNormals.rows(); i++) {
    vertexNormals.row(i) = vertexNormals.row(i).normalized();
  }
}

Eigen::RowVector3f TriangleMesh::getMeshMean() const {
  Eigen::RowVector3f mean = V.colwise().mean();
  return mean;
}

Sphere::Sphere(const Matrix4f T, float radius) : TriangleMesh(T), radius(radius) {
  createSphere();
}

void Sphere::createSphere() {
  // Creates a sphere by starting with a tetrahedron, then subdivides the triangles
  // a few times while normalizing vertices, resulting in an approximation of a sphere.
  const double X = 0.5;
  const double Z = 1.0;
  const Vector3d vdata[12] = {
      {-X, Z, 0.0}, {X, Z, 0.0}, {-X, -Z, 0.0}, {X, -Z, 0.0}, {0.0, X, Z}, {0.0, -X, Z}, {0.0, X, -Z}, {0.0, -X, -Z}, {Z, 0.0, X}, {-Z, 0.0, X}, {Z, 0.0, -X}, {-Z, 0.0, -X}};
  uint32_t faceIndices[20][3] = {
      {0, 4, 1}, {0, 9, 4}, {9, 5, 4}, {4, 5, 8}, {4, 8, 1}, {8, 10, 1}, {8, 3, 10}, {5, 3, 8}, {5, 2, 3}, {2, 7, 3}, {7, 10, 3}, {7, 6, 10}, {7, 11, 6}, {11, 0, 6}, {0, 1, 6}, {6, 1, 10}, {9, 0, 11}, {9, 11, 2}, {9, 2, 5}, {7, 2, 11}};
  F.resize(20, 3);
  for (int i = 0; i < 20; i++) {
    for (int j = 0; j < 3; j++) {
      F(i, j) = faceIndices[i][j];
    }
  }
  V.resize(12, 3);
  for (int i = 0; i < 12; i++) {
    V.row(i) = RowVector3f(vdata[i][0], vdata[i][1], vdata[i][2]).normalized() * radius;
  }

  for (int i = 0; i < 2; i++)
    subdivide();
  computeNormals();
}

void Sphere::subdivide() {
  RowMatrixf newVertices(V.rows() + 3 * F.rows(), 3);
  RowMatrixi newFaces(F.rows() * 4, 3);
  newVertices.block(0, 0, V.rows(), 3) = V.block(0, 0, V.rows(), 3);
  RowVector3f origin = RowVector3f::Zero();
  int nextVertexIndex = V.rows();
  for (int i = 0; i < F.rows(); i++) {
    TriangleFace triangle = F.row(i);
    RowVector3f a = V.row(triangle[0]);
    RowVector3f b = V.row(triangle[1]);
    RowVector3f c = V.row(triangle[2]);

    // Create new vertices.
    RowVector3f d = ((a + b) * 0.5).normalized() * radius;
    RowVector3f e = ((b + c) * 0.5).normalized() * radius;
    RowVector3f f = ((c + a) * 0.5).normalized() * radius;
    newVertices.row(nextVertexIndex) = d;
    newVertices.row(nextVertexIndex + 1) = e;
    newVertices.row(nextVertexIndex + 2) = f;

    // Add faces.
    TriangleFace faceADF(triangle[0], nextVertexIndex, nextVertexIndex + 2);
    TriangleFace faceDBE(nextVertexIndex, triangle[1], nextVertexIndex + 1);
    TriangleFace faceCFE(triangle[2], nextVertexIndex + 2, nextVertexIndex + 1);
    TriangleFace faceDEF(nextVertexIndex, nextVertexIndex + 1, nextVertexIndex + 2);
    newFaces.row(i) = faceADF;
    newFaces.row(F.rows() + i) = faceDBE;
    newFaces.row(F.rows() * 2 + i) = faceCFE;
    newFaces.row(F.rows() * 3 + i) = faceDEF;

    nextVertexIndex += 3;
  }
  F = newFaces;
  V = newVertices;
}

Mesh::Mesh(const std::string& meshFile, const Matrix4f& T, float scale) : TriangleMesh(T) {
  happly::PLYData plyIn(meshFile);
  const auto& vertices = plyIn.getVertexPositions();
  const auto& propertyNames = plyIn.getElement("vertex").getPropertyNames();
  std::vector<std::array<unsigned char, 3>> colors;

  if (std::find(propertyNames.begin(), propertyNames.end(), "red") != propertyNames.end()) {
    colorsFromFile = true;
    colors = plyIn.getVertexColors();
    vertexColors.resize(colors.size(), 3);
  }

  V.resize(vertices.size(), 3);
  for (int i = 0; i < vertices.size(); i++) {
    V(i, 0) = float(vertices[i][0]) * scale;
    V(i, 1) = float(vertices[i][1]) * scale;
    V(i, 2) = float(vertices[i][2]) * scale;

    if (colorsFromFile) {
      vertexColors(i, 0) = colors[i][0];
      vertexColors(i, 1) = colors[i][1];
      vertexColors(i, 2) = colors[i][2];
    }
  }
  auto faces = plyIn.getFaceIndices<size_t>();
  F.resize(faces.size(), 3);
  for (int i = 0; i < faces.size(); i++) {
    for (int j = 0; j < 3; j++) {
      F(i, j) = faces[i][j];
    }
  }
  computeNormals();
}

Mesh::~Mesh() {
}

} // namespace geometry
