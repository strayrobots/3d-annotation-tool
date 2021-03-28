#include <3rdparty/happly.h>
#include "views/view.h"
#include "shader_utils.h"
#include <eigen3/Eigen/Dense>

namespace views {

using namespace Eigen;

using RowMatrixf = Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor>;
using RowMatrixi = Eigen::Matrix<uint32_t, Eigen::Dynamic, 3, Eigen::RowMajor>;
using TriangleFace = Eigen::Matrix<uint32_t, 1, 3, Eigen::RowMajor>;

class TriangleMesh {
protected:
  // Transforms.
  Eigen::Matrix4f transform;

  // Vertex data.
  RowMatrixf V; // Vertices.
  RowMatrixi F;
  RowMatrixf vertexNormals;
  Eigen::Matrix<float, Eigen::Dynamic, 6, Eigen::RowMajor> vertexData;

  // Rendering.
  bgfx::VertexBufferHandle vertexBuffer;
  bgfx::IndexBufferHandle indexBuffer;
  bgfx::VertexLayout layout;
public:
  TriangleMesh(const Matrix4f T = Matrix4f::Identity()) : transform(T) {
    layout.begin()
      .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
      .add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float, true)
      .end();
  }

  const Matrix4f& getTransform() const { return transform; }

  void setRotation(const Matrix3f& rotation) {
    transform.block<3, 3>(0, 0) = rotation;
  }

  void setTransform(const Matrix4f& T) {
    transform = T;
  }

  void render() const {
    bgfx::setTransform(transform.data());
    bgfx::setVertexBuffer(0, vertexBuffer);
    bgfx::setIndexBuffer(indexBuffer);
  }

  const RowMatrixf& vertices() { return V; }
  const RowMatrixi& indices() { return F; }
protected:
  void computeNormals() {
    RowMatrixf faceNormals = RowMatrixf::Zero(F.rows(), 3);
    for (int i=0; i < F.rows(); i++) {
      auto vertex_indices = F.row(i);
      auto vertex1 = V.row(vertex_indices[0]);
      auto vertex2 = V.row(vertex_indices[1]);
      auto vertex3 = V.row(vertex_indices[2]);

      Eigen::RowVector3f a = (vertex1 - vertex2).normalized();
      Eigen::RowVector3f b = (vertex3 - vertex2).normalized();
      faceNormals.row(i) = a.cross(b);
    }
    vertexNormals.resize(V.rows(), 3);
    for (int i=0; i < F.rows(); i++) {
      auto vertex_indices = F.row(i);
      Eigen::RowVector3f normal = faceNormals.row(i);
      vertexNormals.row(vertex_indices[0]) += normal;
      vertexNormals.row(vertex_indices[1]) += normal;
      vertexNormals.row(vertex_indices[2]) += normal;
    }

    for (int i=0; i < vertexNormals.rows(); i++) {
      vertexNormals.row(i) = vertexNormals.row(i).normalized();
    }
  }

  void packVertexData() {
    vertexData.resize(V.rows(), 6);
    for (int i=0; i < V.rows(); i++) {
      auto vertex = V.row(i);
      auto normal = vertexNormals.row(i);
      vertexData.block<1, 3>(i, 0) = V.row(i);
      vertexData.block<1, 3>(i, 3) = normal;
    }
  }
};

class Sphere : public TriangleMesh {
private:
  float radius;
public:
  Sphere(const Matrix4f T, float radius) : TriangleMesh(T), radius(radius) {
    createSphere();
  }

private:
  void createBuffers() {
    indexBuffer = bgfx::createIndexBuffer(bgfx::makeRef(F.data(), F.rows() * F.cols() * sizeof(uint32_t)), BGFX_BUFFER_INDEX32);
    vertexBuffer = bgfx::createVertexBuffer(bgfx::makeRef(vertexData.data(), vertexData.rows() * vertexData.cols() * sizeof(float)), layout);
  }

  void createSphere() {
    // Creates a sphere by starting with a tetrahedron, then subdivides the triangles
    // a few times while normalizing vertices, resulting in an approximation of a sphere.
    const double X = 0.5;
    const double Z = 1.0;
    const Vector3d vdata[12] = {
        { -X, 0.0, Z}, { X, 0.0, Z }, { -X, 0.0, -Z }, { X, 0.0, -Z },
        { 0.0, Z, X }, { 0.0, Z, -X }, { 0.0, -Z, X }, { 0.0, -Z, -X },
        { Z, X, 0.0 }, { -Z, X, 0.0 }, { Z, -X, 0.0 }, { -Z, -X, 0.0 }
    };
    uint32_t faceIndices[20][3] = {
        { 0, 4, 1}, { 0, 9, 4 }, { 9, 5, 4 }, { 4, 5, 8 }, { 4, 8, 1 },
        { 8, 10, 1 }, { 8, 3, 10 }, { 5, 3, 8 }, { 5, 2, 3 }, { 2, 7, 3 },
        { 7, 10, 3 }, { 7, 6, 10 }, { 7, 11, 6 }, { 11, 0, 6 }, { 0, 1, 6 },
        { 6, 1, 10 }, { 9, 0, 11 }, { 9, 11, 2 }, { 9, 2, 5 }, { 7, 2, 11 }
    };
    F.resize(20, 3);
    for (int i=0; i< 20; i++) {
      for (int j=0; j < 3; j++) {
        F(i, j) = faceIndices[i][j];
      }
    }
    V.resize(12, 3);
    for (int i=0; i < 12; i++) {
      V.row(i) = RowVector3f(vdata[i][0], vdata[i][1], vdata[i][2]).normalized() * radius;
    }

    for (int i=0; i < 2; i++)
      subdivide();
    computeNormals();
    packVertexData();
    createBuffers();
  }

  void subdivide() {
    RowMatrixf newVertices(V.rows() + 3 * F.rows(), 3);
    RowMatrixi newFaces(F.rows() * 4, 3);
    newVertices.block(0, 0, V.rows(), 3) = V.block(0, 0, V.rows(), 3);
    RowVector3f origin = RowVector3f::Zero();
    int nextVertexIndex = V.rows();
    for (int i=0; i < F.rows(); i++) {
      TriangleFace triangle = F.row(i);
      RowVector3f a = V.row(triangle[0]);
      RowVector3f b = V.row(triangle[1]);
      RowVector3f c = V.row(triangle[2]);

      // Create new V.
      RowVector3f d = ((a + b) * 0.5).normalized() * radius;
      RowVector3f e = ((b + c) * 0.5).normalized() * radius;
      RowVector3f f = ((c + a) * 0.5).normalized() * radius;
      newVertices.row(nextVertexIndex) = d;
      newVertices.row(nextVertexIndex+1) = e;
      newVertices.row(nextVertexIndex+2) = f;

      // Add vertices.
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
};

class Mesh : public TriangleMesh {
public:
  Mesh(const std::string& meshFile) : TriangleMesh() {
    happly::PLYData plyIn(meshFile);
    const auto& vertices = plyIn.getVertexPositions();
    V.resize(vertices.size(), 3);
    for (int i=0; i < vertices.size(); i++) {
      V(i, 0) = float(vertices[i][0]);
      V(i, 2) = float(vertices[i][1]);
      V(i, 1) = float(vertices[i][2]);
    }
    auto faces = plyIn.getFaceIndices<size_t>();
    F.resize(faces.size(), 3);
    for (int i=0; i < faces.size(); i++) {
      for (int j=0; j < 3; j++) {
        F(i, j) = faces[i][j];
      }
    }
    subtractMean();
    computeNormals();
    packVertexData();

    indexBuffer = bgfx::createIndexBuffer(bgfx::makeRef(F.data(), F.rows() * F.cols() * sizeof(uint32_t)), BGFX_BUFFER_INDEX32);
    vertexBuffer = bgfx::createVertexBuffer(bgfx::makeRef(vertexData.data(), vertexData.rows() * vertexData.cols() * sizeof(float)), layout);
  }

  ~Mesh() {
    bgfx::destroy(indexBuffer);
    bgfx::destroy(vertexBuffer);
  }

protected:
  void subtractMean() {
    Eigen::RowVector3f mean = V.colwise().mean();
    for (int i=0; i < V.rows(); i++) {
      V.row(i) -= mean; // Center.
    }
  }
};

class MeshView : public views::View {
private:
  bgfx::ProgramHandle program;
  std::vector<std::shared_ptr<TriangleMesh>> objects;
  float proj[16];
  float view[16];
public:
  MeshView() {
    program = shader_utils::loadProgram("vs_mesh", "fs_mesh");
  }

  ~MeshView() {
    bgfx::destroy(program);
  }

  void addObject(std::shared_ptr<TriangleMesh> obj) {
    objects.push_back(obj);
  }

  virtual void render(const Camera& camera) override {
    bgfx::setViewRect(0, 0, 0, 800, 600);
    bgfx::touch(0);

    auto position = camera.getPosition();
    auto cameraUp = camera.getUpVector();

    const bx::Vec3 at  = { 0.0f, 0.0f, 0.0f };
    const bx::Vec3 eye = { position[0], position[1], position[2] };
    const bx::Vec3 up = { cameraUp[0], cameraUp[1], cameraUp[2] };
    bx::mtxProj(proj, camera.fov, float(800)/float(600), 0.1f, 25.0f, bgfx::getCaps()->homogeneousDepth);
    bx::mtxLookAt(view, eye, at, up, bx::Handness::Left);

    bgfx::setViewTransform(0, view, proj);

    for (const auto& object : objects) {
      object->render();
      bgfx::setState(BGFX_STATE_DEFAULT | BGFX_STATE_CULL_CW);
      bgfx::submit(0, program);
    }
  }
};

}
