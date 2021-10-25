#pragma once
#include <iostream>
#include <memory>
#include <optional>
#include <vector>
#include "views/view.h"
#include "scene_model.h"
#include "view_context_3d.h"
#include "timeline.h"

#include "shader_utils.h"
#include <bgfx/bgfx.h>

namespace views {

using namespace geometry;

const float PointRadius = 0.1;
const uint16_t InstanceStride = sizeof(float) * 8;

static const uint16_t indices[] = { 0, 1, 2, 0, 2, 3 };
static const float circleVertices[20] = {
  PointRadius,  -PointRadius, 0.0,  1.0, -1.0,
  PointRadius,   PointRadius, 0.0,  1.0,  1.0,
  -PointRadius,  PointRadius, 0.0, -1.0,  1.0,
  -PointRadius, -PointRadius, 0.0, -1.0, -1.0
};

class PointView : public views::View3D {
private:
  std::vector<Vector3f> points;

  bgfx::VertexBufferHandle vertexBuffer;
  bgfx::IndexBufferHandle indexBuffer;
  bgfx::VertexLayout vertexLayout;
  bgfx::ProgramHandle program;
  bgfx::UniformHandle u_rotation;
public:
  PointView(int id) : views::View3D(id), points() {
    vertexLayout.begin()
      .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
      .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
      .end();

    auto verticesRef = bgfx::makeRef(&circleVertices[0], sizeof(float) * 20);
    vertexBuffer = bgfx::createVertexBuffer(verticesRef, vertexLayout);
    indexBuffer = bgfx::createIndexBuffer(bgfx::makeRef(indices, sizeof(indices)));
    u_rotation = bgfx::createUniform("u_rotation", bgfx::UniformType::Mat4);

    program = shader_utils::loadProgram("vs_points", "fs_points");
  }

  void addPoint(const Vector3f& point) {
    points.push_back(point);
  }

  void render(const ViewContext3D& context) const {
    if (points.empty()) return;

    const auto camera = context.camera;
    float proj[16];
    bx::mtxProj(proj, camera.fov, float(context.width) / float(context.height), 0.1f, 25.0f, bgfx::getCaps()->homogeneousDepth, bx::Handness::Right);
    Eigen::Matrix4f viewMatrix = camera.getViewMatrix().matrix();
    bgfx::setViewTransform(viewId, viewMatrix.data(), proj);

    Eigen::Matrix4f rotation = Matrix4f::Identity();
    rotation.block<3, 3>(0, 0) = Eigen::Matrix3f(camera.getOrientation());
    // u_rotation rotates the vertices of the circle and keeps the disks facing
    // towards the camera, regardless of the viewpoint.
    bgfx::setUniform(u_rotation, rotation.data(), 1);

    bgfx::InstanceDataBuffer buffer;

    int spots = bgfx::getAvailInstanceDataBuffer(points.size(), InstanceStride);
    int instanceCount = std::min(int(points.size()), spots);

    bgfx::allocInstanceDataBuffer(&buffer, instanceCount, InstanceStride);
    uint8_t* instanceData = (uint8_t*)buffer.data;

    for (int i=0; i < instanceCount; i++) {
      float* vectors = (float*)instanceData;
      const Vector3f& point = points[i];
      vectors[0] = float(point[0]);
      vectors[1] = float(point[1]);
      vectors[2] = float(point[2]);

      vectors[4] = 1.0f;
      vectors[5] = 0.5f;
      vectors[6] = 0.5f;
      vectors[7] = 1.0f;
      instanceData += InstanceStride;
    }

    bgfx::setVertexBuffer(0, vertexBuffer);
    bgfx::setIndexBuffer(indexBuffer);
    bgfx::setInstanceDataBuffer(&buffer);
    bgfx::setState(BGFX_STATE_DEFAULT | BGFX_STATE_BLEND_ALPHA);
    bgfx::submit(viewId, program);
  }
};

class AddRectangleView : public views::View3D {
private:
  SceneModel& sceneModel;
  Timeline& timeline;
  std::vector<Eigen::Vector3f> points;
  Vector3f position = Vector3f::Zero();
  Quaternionf orientation;
  Vector2f dimensions = Vector2f::Zero();
  Vector3f normal = Vector3f::Zero();
  std::optional<Vector3f> pointingAt;

  PointView pointView;
public:
  AddRectangleView(SceneModel& model, Timeline& timeline, int viewId = 0);
  bool leftButtonUp(const ViewContext3D& viewContext) override;
  bool leftButtonDown(const ViewContext3D& context) override;
  bool mouseMoved(const ViewContext3D& context) override;
  void render(const ViewContext3D& context) const;
};
} // namespace views
