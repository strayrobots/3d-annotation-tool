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
  Matrix4f yTransform;
  Matrix4f zTransform;

  const Vector4f lightDir = Vector4f(0.0, 1.0, -1.0, 1.0);
  // Rendering.
  bgfx::ProgramHandle program;
  bgfx::UniformHandle u_color, u_lightDir;
public:
  TranslateControl()  {
    auto y = Transform<float, 3, Eigen::Affine>::Identity();
    auto z = Transform<float, 3, Eigen::Affine>::Identity();
    y.rotate(AngleAxisf(M_PI/2.0, Vector3f(0.0, 0.0, 1.0)));
    z.rotate(AngleAxisf(-M_PI/2.0, Vector3f(0.0, 1.0, 0.0)));
    yTransform = y.matrix();
    zTransform = z.matrix();

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

  bool hitTest(const ViewContext3D& viewContext) const override {
    const Vector3f& cameraOrigin = viewContext.camera.getPosition();
    const Vector3f& rayDirection = viewContext.camera.computeRayWorld(viewContext.width, viewContext.height,
        viewContext.mousePositionX, viewContext.mousePositionY);
    auto hit = rtAxisMesh->traceRay(cameraOrigin, rayDirection);
    if (hit.has_value()) {
      std::cout << "hit x-axis" << std::endl;
    }
    return hit.has_value();
  }

  void setPosition(const Vector3f& newPos) {
    position = newPos;
  }

  void render(const Camera& camera) const override {
    bgfx::setUniform(u_lightDir, lightDir.data(), 1);
    bgfx::setUniform(u_color, xAxisColor.data(), 1);
    bgfx::setTransform(xAxisDrawable->getTransform().data(), 1);
    xAxisDrawable->setDrawingGeometry();
    bgfx::submit(0, program);

    bgfx::setUniform(u_lightDir, lightDir.data(), 1);
    bgfx::setUniform(u_color, yAxisColor.data(), 1);
    bgfx::setTransform(yTransform.data(), 1);
    xAxisDrawable->setDrawingGeometry();
    bgfx::submit(0, program);

    bgfx::setUniform(u_lightDir, lightDir.data(), 1);
    bgfx::setUniform(u_color, zAxisColor.data(), 1);
    bgfx::setTransform(zTransform.data(), 1);
    xAxisDrawable->setDrawingGeometry();
    bgfx::submit(0, program);
  }
};

}
