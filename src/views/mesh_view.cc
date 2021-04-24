#include <omp.h>
#include <3rdparty/happly.h>
#include <eigen3/Eigen/Dense>
#include <bx/math.h>
#include "views/view.h"
#include "shader_utils.h"
#include "views/mesh_view.h"

namespace views
{

using namespace Eigen;

using RowMatrixf = Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor>;
using RowMatrixi = Eigen::Matrix<uint32_t, Eigen::Dynamic, 3, Eigen::RowMajor>;
using TriangleFace = Eigen::Matrix<uint32_t, 1, 3, Eigen::RowMajor>;

MeshDrawable::MeshDrawable(std::shared_ptr<geometry::TriangleMesh> m, const Vector4f& c) : mesh(m), defaultColor(c)
{
  layout.begin()
      .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
      .add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float, true)
      .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
      .end();
  packVertexData();
  createBuffers();
}

MeshDrawable::~MeshDrawable()
{
  bgfx::destroy(indexBuffer);
  bgfx::destroy(vertexBuffer);
}

void MeshDrawable::createBuffers()
{
  const auto& F = mesh->faces();
  indexBuffer = bgfx::createIndexBuffer(bgfx::makeRef(F.data(), F.rows() * F.cols() * sizeof(uint32_t)), BGFX_BUFFER_INDEX32);
  vertexBuffer = bgfx::createVertexBuffer(bgfx::makeRef(vertexData.data(), vertexData.rows() * vertexData.cols() * sizeof(float)), layout);
}

void MeshDrawable::packVertexData()
{
  const auto& F = mesh->faces();
  const auto& V = mesh->vertices();
  const auto& N = mesh->getVertexNormals();
  const auto& vertexColors = mesh->getVertexColors();
  bool colorsFromFile = mesh->colorsFromFile;
  vertexData.resize(V.rows(), 7);

#pragma omp parallel for
  for (int i = 0; i < V.rows(); i++)
  {
    float vertexColor;
    if (colorsFromFile)
    {
      std::memcpy(&vertexColor, &vertexColors[i], sizeof(vertexColors[i]));
    }
    else
    {
      //Vector4f to 4 concatenated uint8_t representing the abgr color value
      uint32_t uint32defaultColor = ((uint8_t)(255 * defaultColor[3]) << 24) + ((uint8_t)(255 * defaultColor[2]) << 16) + ((uint8_t)(255 * defaultColor[1]) << 8) + (uint8_t)(255 * defaultColor[0]);
      std::memcpy(&vertexColor, &uint32defaultColor, sizeof(uint32defaultColor));
    }

    vertexData.block<1, 3>(i, 0) = V.row(i);
    vertexData.block<1, 3>(i, 3) = N.row(i);
    vertexData.block<1, 1>(i, 6) << vertexColor;
  }
}

void MeshDrawable::setDrawingGeometry() const
{
  bgfx::setTransform(mesh->getTransform().data());
  bgfx::setVertexBuffer(0, vertexBuffer);
  bgfx::setIndexBuffer(indexBuffer);
}

MeshView::MeshView(int width, int height) : View(width, height), lightDir(0.0, 1.0, -1.0, 1.0)
{
  u_lightDir = bgfx::createUniform("u_light_dir", bgfx::UniformType::Vec4);
  program = shader_utils::loadProgram("vs_mesh", "fs_mesh");
}

MeshView::~MeshView()
{
  bgfx::destroy(program);
  bgfx::destroy(u_lightDir);
  bgfx::destroy(u_color);
}

void MeshView::addObject(std::shared_ptr<MeshDrawable> obj)
{
  objects.push_back(obj);
}

void MeshView::render(const Camera& camera) const
{
  float proj[16];
  float view[16];

  bgfx::touch(0);

  auto position = camera.getPosition();
  auto lookat = camera.getLookat();
  auto cameraUp = camera.getUpVector();

  const bx::Vec3 at = {lookat[0], lookat[1], lookat[2]};
  const bx::Vec3 eye = {position[0], position[1], position[2]};
  const bx::Vec3 up = {cameraUp[0], cameraUp[1], cameraUp[2]};
  bx::mtxProj(proj, camera.fov, float(width) / float(height), 0.1f, 25.0f, bgfx::getCaps()->homogeneousDepth, bx::Handness::Right);
  bx::mtxLookAt(view, eye, at, up, bx::Handness::Right);

  bgfx::setViewTransform(0, view, proj);

  bgfx::setUniform(u_lightDir, lightDir.data(), 1);
  for (const auto& object : objects)
  {
    object->setDrawingGeometry();
    bgfx::setState(BGFX_STATE_DEFAULT | BGFX_STATE_CULL_CW);
    bgfx::submit(0, program);
  }
}

} // namespace views
