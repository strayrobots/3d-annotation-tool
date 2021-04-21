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
  std::shared_ptr<views::MeshDrawable> xAxisControl;
  std::shared_ptr<views::MeshDrawable> yAxisControl;
  std::shared_ptr<views::MeshDrawable> zAxisControl;
  Vector3f position = Vector3f::Zero();
  std::optional<std::function<void(double, const Vector3f&)>> callback;

  std::unique_ptr<geometry::RayTraceMesh> rtAxisMesh;

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
    auto yAxisMesh = std::make_shared<geometry::Mesh>("../assets/x_axis.ply", yTransform, 0.5);
    auto zAxisMesh = std::make_shared<geometry::Mesh>("../assets/x_axis.ply", zTransform, 0.5);
    xAxisControl = std::make_shared<views::MeshDrawable>(xAxisMesh, Vector4f(1.0, 0.2, 0.2, 1.0));
    yAxisControl = std::make_shared<views::MeshDrawable>(yAxisMesh, Vector4f(0.2, 1.0, 0.2, 1.0));
    zAxisControl = std::make_shared<views::MeshDrawable>(zAxisMesh, Vector4f(0.2, 0.2, 1.0, 1.0));
    rtAxisMesh = std::make_unique<geometry::RayTraceMesh>(*xAxisMesh);

    u_lightDir = bgfx::createUniform("u_light_dir", bgfx::UniformType::Vec4);
    u_color = bgfx::createUniform("u_color", bgfx::UniformType::Vec4);
    program = shader_utils::loadProgram("vs_mesh", "fs_mesh");
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
    auto position = camera.getPosition();
    auto lookat = camera.getLookat();
    auto cameraUp = camera.getUpVector();

    float proj[16];
    float view[16];
    const bx::Vec3 at  = { lookat[0], lookat[1], lookat[2] };
    const bx::Vec3 eye = { position[0], position[1], position[2] };
    const bx::Vec3 up = { cameraUp[0], cameraUp[1], cameraUp[2] };
    bx::mtxProj(proj, camera.fov, float(800)/float(600), 0.1f, 25.0f, bgfx::getCaps()->homogeneousDepth, bx::Handness::Right);
    bx::mtxLookAt(view, eye, at, up, bx::Handness::Right);

    bgfx::setViewTransform(0, view, proj);

    bgfx::setUniform(u_lightDir, lightDir.data(), 1);
    xAxisControl->setDrawingGeometry(u_color);
    bgfx::submit(0, program);
    yAxisControl->setDrawingGeometry(u_color);
    bgfx::submit(0, program);
    zAxisControl->setDrawingGeometry(u_color);
    bgfx::submit(0, program);
  }
};

}
