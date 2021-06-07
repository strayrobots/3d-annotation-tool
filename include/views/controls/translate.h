#pragma once
#include <optional>
#include <functional>
#include <memory>
#include <bgfx/bgfx.h>
#include <eigen3/Eigen/Geometry>
#include "geometry/mesh.h"
#include "geometry/ray_trace_mesh.h"
#include "views/mesh_view.h"
#include "views/controls/control.h"

namespace views::controls {

class TranslateControl : public views::controls::Control {
private:
  std::shared_ptr<views::MeshDrawable> xAxisDrawable;
  std::function<void(const Vector3f&)> callback;

  std::unique_ptr<geometry::RayTraceMesh> rtAxisMesh;

  Vector4f xAxisColor = Vector4f(1.0, 0.2, 0.2, 1.0);
  Vector4f yAxisColor = Vector4f(0.2, 1.0, 0.2, 1.0);
  Vector4f zAxisColor = Vector4f(0.2, 0.2, 1.0, 1.0);
  Quaternionf yRotation;
  Quaternionf zRotation;
  Transform<float, 3, Eigen::Affine> currentTransform = Transform<float, 3, Eigen::Affine>::Identity();
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
  TranslateControl(int viewId, std::function<void(const Vector3f&)> cb);
  ~TranslateControl();
  void setCallback(std::function<void(const Vector3f&)> cb);
  bool leftButtonDown(const ViewContext3D& viewContext) override;
  bool leftButtonUp(const ViewContext3D& viewContext) override;
  bool mouseMoved(const ViewContext3D& viewContext) override;
  Vector3f getPosition() const { return currentTransform.translation(); };
  void setPosition(const Vector3f& newPos);
  void setOrientation(const Quaternionf& orientation);
  void render(const Camera& camera) const;
};

} // namespace views::controls
