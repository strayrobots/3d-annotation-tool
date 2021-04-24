#pragma once
#include <eigen3/Eigen/Dense>

using namespace Eigen;

using RowMatrixf = Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor>;
using RowMatrixi = Eigen::Matrix<uint32_t, Eigen::Dynamic, 3, Eigen::RowMajor>;
using TriangleFace = Eigen::Matrix<uint32_t, 1, 3, Eigen::RowMajor>;

namespace geometry {

class TriangleMesh {
protected:
  // Transforms.
  Eigen::Matrix4f transform;

  // Vertex data.
  RowMatrixf V; // Vertices.
  RowMatrixi F;
  RowMatrixf vertexNormals;
public:
  TriangleMesh(const Matrix4f T = Matrix4f::Identity());

  const Matrix4f& getTransform() const;
  void setTranslation(const Vector3f& t);
  void setRotation(const Matrix3f& rotation);
  void setTransform(const Matrix4f& T);

  const RowMatrixf& vertices() const { return V; }
  const RowMatrixi& faces() const { return F; }
  const RowMatrixf& getVertexNormals() const { return vertexNormals; }
  Eigen::RowVector3f getMeshMean() const;
protected:
  void computeNormals();
};

class Sphere : public TriangleMesh {
private:
  float radius;
public:
  Sphere(const Matrix4f T, float radius);
private:
  void createSphere();
  void subdivide();
};

class Mesh : public TriangleMesh {
public:
  Mesh(const std::string& meshFile, const Matrix4f& T = Matrix4f::Identity(), float scale=1.0);
  ~Mesh();
};
}
