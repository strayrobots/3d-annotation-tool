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

class RotateControl : public views::controls::Control {
private:
  std::shared_ptr<views::MeshDrawable> diskDrawable;
  std::function<void(const Transform<float, 3, Eigen::Affine>&)> callback;

  std::unique_ptr<geometry::RayTraceMesh> rtDiskMesh;

  Vector4f xDiskColor = Vector4f(1.0, 0.2, 0.2, 0.1);
  Vector4f yDiskColor = Vector4f(0.2, 1.0, 0.2, 0.1);
  Vector4f zDiskColor = Vector4f(0.2, 0.2, 1.0, 0.1);
  Quaternionf rotation_Fx;
  Quaternionf rotation_Fy;
  Quaternionf rotation_Fz;
  Transform<float, 3, Eigen::Affine> currentTransform_WF = Transform<float, 3, Eigen::Affine>::Identity();
  Transform<float, 3, Eigen::Affine> transform_Wx;
  Transform<float, 3, Eigen::Affine> transform_Wy;
  Transform<float, 3, Eigen::Affine> transform_Wz;

  const Vector4f lightDir = Vector4f(0.0, 1.0, -1.0, 1.0);
  // Rendering.
  bgfx::ProgramHandle program;
  bgfx::UniformHandle u_color, u_lightDir;
  int activeAxis = -1;

  Vector3f rotationAxis;
  Vector3f dragPoint;

public:
  RotateControl(int viewId, std::function<void(const Transform<float, 3, Eigen::Affine>&)> cb);
  ~RotateControl();
  void setCallback(std::function<void(const Vector3f&)> cb);
  bool leftButtonDown(const ViewContext3D& viewContext) override;
  bool leftButtonUp(const ViewContext3D& viewContext) override;
  bool mouseMoved(const ViewContext3D& viewContext) override;
  Vector3f getPosition() const { return currentTransform_WF.translation(); };
  void setPosition(const Vector3f& newPos);
  void setOrientation(const Quaternionf& orientation);
  void render(const Camera& camera) const;
};

} // namespace views::controls
