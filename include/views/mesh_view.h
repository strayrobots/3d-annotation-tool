#include "views/view.h"
#include "happly.h"
#include "shader_utils.h"
#include <eigen3/Eigen/Dense>

namespace views {

using namespace Eigen;

using RowMatrixf = Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor>;
using RowMatrixi = Eigen::Matrix<uint32_t, Eigen::Dynamic, 3, Eigen::RowMajor>;

class MeshView : public views::View {
private:
  RowMatrixf V; // Vertices.
  RowMatrixi F;
  RowMatrixf vertexNormals;
  Eigen::Matrix<float, Eigen::Dynamic, 6, Eigen::RowMajor> vertexData;

  bgfx::VertexBufferHandle vertexBuffer;
  bgfx::IndexBufferHandle indexBuffer;
  bgfx::VertexLayout layout;
  bgfx::ProgramHandle program;

public:
  MeshView(const std::string& mesh) {
    happly::PLYData plyIn(mesh);
    auto vertices = plyIn.getVertexPositions();
    V.resize(vertices.size(), 3);
    for (int i=0; i < vertices.size(); i++) {
      auto vertex = V.row(i);
      for (int j=0; j < 3; j++) {
        vertex[j] = float(vertices[i][j]);
      }
    }
    auto faces = plyIn.getFaceIndices<size_t>();
    F.resize(faces.size(), 3);
    for (int i=0; i < faces.size(); i++) {
      for (int j=0; j < 3; j++) {
        F(i, j) = faces[i][j];
      }
    }
    computeNormals();

    layout.begin()
      .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
      .add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float, true)
      .end();

    indexBuffer = bgfx::createIndexBuffer(bgfx::makeRef(F.data(), F.rows() * F.cols() * sizeof(uint32_t)), BGFX_BUFFER_INDEX32);

    vertexData.resize(V.rows(), 6);
    for (int i=0; i < V.rows(); i++) {
      auto vertex = V.row(i);
      auto normal = vertexNormals.row(i);
      vertexData.block<1, 3>(i, 0) = V.row(i);
      vertexData.block<1, 3>(i, 3) = normal;
    }
    vertexBuffer = bgfx::createVertexBuffer(bgfx::makeRef(vertexData.data(), vertexData.rows() * vertexData.cols() * sizeof(float)), layout);

    program = shader_utils::loadProgram("vs_mesh", "fs_mesh");
  }

  ~MeshView() {
    bgfx::destroy(program);
    bgfx::destroy(indexBuffer);
    bgfx::destroy(vertexBuffer);
  }

  virtual void render(const Vector3f& eyePosition, const Matrix3f &rotation) override {
    bgfx::setViewRect(0, 0, 0, 800, 600);
    const bx::Vec3 at  = { 0.0f, 0.0f, 0.0f };
    const bx::Vec3 eye = { eyePosition[0], eyePosition[1], eyePosition[2] };
    float view[16];
    bx::mtxLookAt(view, eye, at);

    float proj[16];
    bx::mtxProj(proj, 30.0f, float(800)/float(600), 0.1f, 25.0f, bgfx::getCaps()->homogeneousDepth);

    bgfx::setViewTransform(0, view, proj);

    float modelView[16];
    for (int i=0; i<3; i++) {
      int row = i * 4;
      modelView[row] = rotation(i, 0);
      modelView[row + 1] = rotation(i, 1);
      modelView[row + 2] = rotation(i, 2);
    }
    modelView[15] = 1.0;

    bgfx::touch(0);

    Eigen::Matrix4f transform = Matrix4f::Identity();
    transform.block<3, 3>(0, 0) = rotation;
    bgfx::setTransform(transform.transpose().data());
    bgfx::setVertexBuffer(0, vertexBuffer);
    bgfx::setIndexBuffer(indexBuffer);

    bgfx::setState(BGFX_STATE_DEFAULT | BGFX_STATE_CULL_CCW);
    bgfx::submit(0, program);
  }

protected:
  void computeNormals() {
    Eigen::RowVector3f mean = V.colwise().mean();
    for (int i=0; i < V.rows(); i++) {
      V.row(i) -= mean; // Center.
    }
    RowMatrixf faceNormals = RowMatrixf::Zero(F.rows(), 3);
    for (int i=0; i < F.rows(); i++) {
      auto vertex_indices = F.row(i);
      auto vertex1 = V.row(vertex_indices[0]);
      auto vertex2 = V.row(vertex_indices[1]);
      auto vertex3 = V.row(vertex_indices[2]);

      Eigen::RowVector3f a = (vertex2 - vertex1).normalized();
      Eigen::RowVector3f b = (vertex3 - vertex1).normalized();
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
};

}
