#include "views/view.h"
#include "happly.h"
#include "shader_utils.h"
#include <eigen3/Eigen/Dense>

namespace views {

Eigen::RowVector3f toVector(std::array<float, 3>& a) {
  return Eigen::RowVector3f(a[0], a[1], a[2]);
}

using RowMatrix = Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor>;
static const float verts[] = {
  -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, -1.0f,
  -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, -1.0f,
   0.5f, -0.5f, 0.0f, 0.0f, 0.0f, -1.0f,
   0.5f,  0.5f, 0.0f, 0.0f, 0.0f, -1.0f
};

static const uint16_t faces[] = {
  0, 3, 1,
  0, 2, 3
};


class MeshView : public views::View {
private:
  RowMatrix V; // Vertices.
  std::vector<std::vector<size_t>> F;
  RowMatrix faceNormals;
  RowMatrix vertexNormals;
  std::vector<std::array<float, 6>> vertexData;

  bgfx::VertexBufferHandle vertexBuffer;
  bgfx::IndexBufferHandle indexBuffer;
  bgfx::VertexLayout layout;
  bgfx::ProgramHandle program;

public:
  MeshView(const std::string& mesh) {
    // happly::PLYData plyIn(mesh);
    // auto vertices = plyIn.getVertexPositions();
    // V.resize(vertices.size(), 3);
    // for (int i=0; i < vertices.size(); i++) {
    //   auto vertex = V.row(i);
    //   for (int j=0; j < 3; j++) {
    //     vertex[j] = float(vertices[i][j]);
    //   }
    // }
    // F = plyIn.getFaceIndices<size_t>();
    // computeNormals();

    layout.begin()
      .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
      .add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float, true)
      .end();

    indexBuffer = bgfx::createIndexBuffer(bgfx::makeRef(faces, 6 * sizeof(uint16_t)));

    //vertexData.resize(V.rows());
    //for (int i=0; i < V.rows(); i++) {
    //  std::array<float, 6> item;
    //  auto vertex = V.row(i);
    //  auto normal = vertexNormals.row(i);
    //  for (int j=0; j < 3; j++) {
    //    item[j] = vertex[j];
    //    item[3 + j] = normal[j];
    //  }
    //}
    //vertexBuffer = bgfx::createVertexBuffer(bgfx::makeRef(vertexData.data(), sizeof(V.rows() * 2 * 3 * sizeof(float))), layout);
    vertexBuffer = bgfx::createVertexBuffer(bgfx::makeRef(verts, 4 * 6 * sizeof(float)), layout);

    program = shader_utils::loadProgram("vs_mesh", "fs_mesh");
  }

  ~MeshView() {
    bgfx::destroy(program);
    bgfx::destroy(indexBuffer);
    bgfx::destroy(vertexBuffer);
  }

  virtual void render() override {
    bgfx::setViewRect(0, 0, 0, uint16_t(800), uint16_t(600));
    //const bx::Vec3 at  = { 0.0f, 0.0f, 0.0f };
    //const bx::Vec3 eye = { 0.0f, 0.0f, 0.5f };
    //float view[16];
    //bx::mtxLookAt(view, eye, at);

    //float proj[16];
    //bx::mtxProj(proj, 60.0f, float(800)/float(600), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);

    //bgfx::setViewTransform(0, view, proj);

    //float modelView[16];
    //bx::mtxRotateY(modelView, 0.0);
    //for (int i=0; i<16; i++) modelView[i] = 0.0;
    //modelView[0] = 1.0;
    //modelView[5] = 1.0;
    //modelView[10] = 1.0;
    //modelView[15] = 1.0;

    bgfx::touch(0);

    //bgfx::setTransform(modelView);
    bgfx::setVertexBuffer(0, vertexBuffer);
    bgfx::setIndexBuffer(indexBuffer);

    bgfx::setState(BGFX_STATE_DEFAULT);
    bgfx::submit(0, program);
  }

protected:
  void computeNormals() {
    faceNormals.resize(F.size(), 3);
    vertexNormals = RowMatrix::Zero(V.rows(), 3);
    for (int i=0; i < F.size(); i++) {
      auto vertex_indices = F[i];
      auto vertex1 = V.row(vertex_indices[0]);
      auto vertex2 = V.row(vertex_indices[1]);
      auto vertex3 = V.row(vertex_indices[2]);

      Eigen::RowVector3f a = vertex2 - vertex1;
      Eigen::RowVector3f b = vertex3 - vertex1;
      faceNormals.row(i) = a.cross(b).normalized();
    }
    Eigen::RowVector3f mean = V.colwise().mean();
    Eigen::RowVector3f max = V.colwise().maxCoeff();
    std::cout << "max: " << max << std::endl;
    std::cout << "min: " << V.colwise().minCoeff() << std::endl;
    for (int i=0; i < V.rows(); i++) {
      V.row(i) = (V.row(i) - mean);
      auto vertex_indices = F[i];
      auto vertex1 = V.row(vertex_indices[0]);
      auto vertex2 = V.row(vertex_indices[1]);
      auto vertex3 = V.row(vertex_indices[2]);

      Eigen::RowVector3f a = vertex2 - vertex1;
      Eigen::RowVector3f b = vertex3 - vertex1;
      Eigen::RowVector3f normal = a.cross(b);
      faceNormals.row(i) = normal;
      vertexNormals.row(vertex_indices[0]) += normal;
      vertexNormals.row(vertex_indices[1]) += normal;
      vertexNormals.row(vertex_indices[1]) += normal;
    }

    for (int i=0; i < V.rows(); i++) {
      vertexNormals.row(i).normalize();
    }
  }
};

}
