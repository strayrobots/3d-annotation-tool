#pragma once
#include <optional>
#include <functional>
#include <memory>
#include <bx/math.h>
#include <bgfx/bgfx.h>
#include "views/view.h"
#include "geometry/mesh.h"
#include "views/mesh_view.h"
#include "shader_utils.h"
#include "views/controls/control.h"

namespace views::controls {

class TranslateControl : public views::controls::Control {
private:
  std::shared_ptr<views::MeshDrawable> xAxisDrawable;
  Vector3f position = Vector3f::Zero();
  std::optional<std::function<void(double, const Vector3f&)>> callback;

  std::unique_ptr<geometry::RayTraceMesh> rtAxisMesh;

  Vector4f xAxisColor = Vector4f(1.0, 0.2, 0.2, 1.0);
  Vector4f yAxisColor = Vector4f(0.2, 1.0, 0.2, 1.0);
  Vector4f zAxisColor = Vector4f(0.2, 0.2, 1.0, 1.0);
  Transform<float, 3, Eigen::Affine> currentTransform = Transform<float, 3, Eigen::Affine>::Identity();;
  Transform<float, 3, Eigen::Affine> yTransform;
  Transform<float, 3, Eigen::Affine> zTransform;

  const Vector4f lightDir = Vector4f(0.0, 1.0, -1.0, 1.0);
  // Rendering.
  bgfx::ProgramHandle program;
  bgfx::UniformHandle u_color, u_lightDir;
  int activeAxis = -1;
  // Point on the chosen axis that is being dragged.
  Vector3f dragPoint;
public:
  TranslateControl()  {
    yTransform = Transform<float, 3, Eigen::Affine>::Identity();
    zTransform = Transform<float, 3, Eigen::Affine>::Identity();
    yTransform.rotate(AngleAxisf(M_PI/2.0, Vector3f(0.0, 0.0, 1.0)));
    zTransform.rotate(AngleAxisf(-M_PI/2.0, Vector3f(0.0, 1.0, 0.0)));

    auto xAxisMesh = std::make_shared<geometry::Mesh>("../assets/x_axis.ply", Matrix4f::Identity(), 0.5);
    xAxisDrawable = std::make_shared<views::MeshDrawable>(xAxisMesh, xAxisColor);
    rtAxisMesh = std::make_unique<geometry::RayTraceMesh>(*xAxisMesh);

    u_lightDir = bgfx::createUniform("u_light_dir", bgfx::UniformType::Vec4);
    u_color = bgfx::createUniform("u_color", bgfx::UniformType::Vec4);
    program = shader_utils::loadProgram("vs_mesh", "fs_mesh");
  }

  ~TranslateControl() {
    bgfx::destroy(program);
    bgfx::destroy(u_lightDir);
    bgfx::destroy(u_color);
  }

  void setCallback(std::function<void(double, const Vector3f&)> cb) { callback = cb; }

  bool leftButtonDown(const ViewContext3D& viewContext) override {
    activeAxis = -1;
    const Vector3f& cameraOrigin = viewContext.camera.getPosition() - currentTransform.translation();
    const Vector3f& rayDirection = viewContext.camera.computeRayWorld(viewContext.width, viewContext.height,
        viewContext.mousePositionX, viewContext.mousePositionY);
    auto hitX = rtAxisMesh->traceRay(cameraOrigin, rayDirection);
    if (hitX.has_value()) {
      activeAxis = 0;
      dragPoint = hitX.value()[0] * Vector3f::UnitX();
    }

    auto hitY = rtAxisMesh->traceRay(yTransform.rotation().transpose() * cameraOrigin, yTransform.rotation().transpose() * rayDirection);
    if (hitY.has_value()) {
      activeAxis = 1;
      dragPoint = hitY.value()[0] * Vector3f::UnitY();
    }

    auto hitZ = rtAxisMesh->traceRay(zTransform.rotation().transpose() * cameraOrigin, zTransform.rotation().transpose() * rayDirection);
    if (hitZ.has_value()) {
      activeAxis = 2;
      dragPoint = hitZ.value()[0] * Vector3f::UnitZ();
    }

    return activeAxis != -1;
  }

  bool leftButtonUp(const ViewContext3D& viewContext) override {
    bool wasActive = activeAxis != -1;
    activeAxis = -1;
    return wasActive;
  }

  bool mouseMoved(const ViewContext3D& viewContext) override {
    if (activeAxis < 0) return false;
    const Vector3f& cameraOrigin = viewContext.camera.getPosition() - currentTransform.translation();
    const Vector3f& rayDirection = viewContext.camera.computeRayWorld(viewContext.width, viewContext.height,
        viewContext.mousePositionX, viewContext.mousePositionY);
    const Vector3f& translation = currentTransform.translation();
    Vector3f change;
    if (activeAxis == 0) {
      float t = -cameraOrigin[2] / rayDirection[2];
      Vector3f pointOnXYPlane = cameraOrigin + t * rayDirection;
      float displacement = pointOnXYPlane[0];
      change = (displacement - dragPoint[0]) * Vector3f::UnitX();
    } else if (activeAxis == 1) {
      float t = -cameraOrigin[2] / rayDirection[2];
      Vector3f pointOnXYPlane = cameraOrigin + t * rayDirection;
      float displacement = pointOnXYPlane[1];
      change = (displacement - dragPoint[1]) * Vector3f::UnitY();
    } else if (activeAxis == 2) {
      float t = -cameraOrigin[0] / rayDirection[0];
      Vector3f pointOnXZPlane = cameraOrigin + t * rayDirection;
      float displacement = pointOnXZPlane[2];
      change = (displacement - dragPoint[2]) * Vector3f::UnitZ();
    }
    currentTransform.translation() += change;
    yTransform.translation() = currentTransform.translation();
    zTransform.translation() = currentTransform.translation();
    return true;
  }

  void setPosition(const Vector3f& newPos) {
    position = newPos;
  }

  void render(const Camera& camera) const override {
    bgfx::setUniform(u_lightDir, lightDir.data(), 1);
    bgfx::setUniform(u_color, xAxisColor.data(), 1);
    bgfx::setTransform(currentTransform.matrix().data(), 1);
    xAxisDrawable->setDrawingGeometry();
    bgfx::submit(0, program);

    bgfx::setUniform(u_lightDir, lightDir.data(), 1);
    bgfx::setUniform(u_color, yAxisColor.data(), 1);
    bgfx::setTransform(yTransform.matrix().data(), 1);
    xAxisDrawable->setDrawingGeometry();
    bgfx::submit(0, program);

    bgfx::setUniform(u_lightDir, lightDir.data(), 1);
    bgfx::setUniform(u_color, zAxisColor.data(), 1);
    bgfx::setTransform(zTransform.matrix().data(), 1);
    xAxisDrawable->setDrawingGeometry();
    bgfx::submit(0, program);
  }
};

}
