#include <bx/math.h>
#include "views/controls/rotate.h"
#include "views/view.h"
#include "geometry/mesh.h"
#include "views/mesh_view.h"
#include "shader_utils.h"
#include "views/controls/control.h"
#include "shader_utils.h"
#include "geometry/mesh.h"
#include <iostream>

namespace views::controls {

RotateControl::RotateControl(int viewId, std::function<void(const Transform<float, 3, Eigen::Affine>&)> cb) : views::controls::Control(viewId), callback(cb) {
  rotation_Fx = AngleAxisf(M_PI / 2.0, Vector3f(0.0, 1.0, 0.0));
  rotation_Fy = AngleAxisf(M_PI / 2.0, Vector3f(1.0, 0.0, 0.0));
  rotation_Fz = AngleAxisf(0.0, Vector3f(1.0, 0.0, 0.0));

  transform_Wx = Transform<float, 3, Eigen::Affine>::Identity();
  transform_Wy = Transform<float, 3, Eigen::Affine>::Identity();
  transform_Wz = Transform<float, 3, Eigen::Affine>::Identity();

  transform_Wx.rotate(rotation_Fx);
  transform_Wy.rotate(rotation_Fy);
  transform_Wz.rotate(rotation_Fz);

  auto diskMesh = std::make_shared<geometry::Mesh>("../assets/disk.ply", Matrix4f::Identity(), 0.001);
  diskDrawable = std::make_shared<views::MeshDrawable>(diskMesh);
  rtDiskMesh = std::make_unique<geometry::RayTraceMesh>(diskMesh);

  u_lightDir = bgfx::createUniform("u_light_dir", bgfx::UniformType::Vec4);
  u_color = bgfx::createUniform("u_color", bgfx::UniformType::Vec4);
  program = shader_utils::loadProgram("vs_mesh", "fs_mesh_uniform");
}

RotateControl::~RotateControl() {
  bgfx::destroy(program);
  bgfx::destroy(u_lightDir);
  bgfx::destroy(u_color);
}

bool RotateControl::leftButtonDown(const ViewContext3D& viewContext) {
  activeAxis = -1;
  const Vector3f& cameraOrigin = (viewContext.camera.getPosition() - currentTransform_WF.translation());
  const Vector3f& rayDirection = viewContext.camera.computeRayWorld(viewContext.width, viewContext.height,
                                                                    viewContext.mousePositionX, viewContext.mousePositionY);

  auto hitX = rtDiskMesh->traceRay(transform_Wx.rotation().transpose() * cameraOrigin, transform_Wx.rotation().transpose() * rayDirection);
  if (hitX.has_value()) {
    activeAxis = 0;
    dragPoint = hitX.value()[0] * Vector3f::UnitX();
    focusPoint = hitX.value()[1] * Vector3f::UnitY() - hitX.value()[0] * Vector3f::UnitZ();
    std::cout << "Hit value " << hitX.value() << std::endl;
    std::cout << "Focuspoint " << focusPoint << std::endl;
  }

  auto hitY = rtDiskMesh->traceRay(transform_Wy.rotation().transpose() * cameraOrigin, transform_Wy.rotation().transpose() * rayDirection);
  if (hitY.has_value()) {
    activeAxis = 1;
    dragPoint = hitY.value()[0] * Vector3f::UnitY();
  }

  auto hitZ = rtDiskMesh->traceRay(transform_Wz.rotation().transpose() * cameraOrigin, transform_Wz.rotation().transpose() * rayDirection);
  if (hitZ.has_value()) {
    activeAxis = 2;
    dragPoint = hitZ.value()[0] * Vector3f::UnitZ();
  }

  return activeAxis != -1;
}

bool RotateControl::leftButtonUp(const ViewContext3D& viewContext) {
  bool wasActive = activeAxis != -1;
  activeAxis = -1;
  return wasActive;
}

bool RotateControl::mouseMoved(const ViewContext3D& viewContext) {
  if (activeAxis < 0) return false;
  const Vector3f& cameraOrigin_F = currentTransform_WF.rotation().transpose() * (viewContext.camera.getPosition() - currentTransform_WF.translation());
  const Vector3f& rayDirection_F = currentTransform_WF.rotation().transpose() * viewContext.camera.computeRayWorld(viewContext.width, viewContext.height,
                                                                                                                   viewContext.mousePositionX, viewContext.mousePositionY);
  Vector3f change;
  Quaternionf rotation;
  Quaternionf currentRotation_WF;

  currentRotation_WF = currentTransform_WF.rotation();

  if (activeAxis == 0) {
    Vector3f pointOnYZPlane = cameraOrigin_F + rayDirection_F * currentTransform_WF.translation().norm();
    pointOnYZPlane[0] = 0.0;
    auto angle = std::acos(pointOnYZPlane.normalized().dot(focusPoint.normalized()));

    std::cout << "Focus close " << pointOnYZPlane << std::endl;
    std::cout << "Angle " << angle << std::endl;
    focusPoint = pointOnYZPlane;
    rotation = AngleAxisf(angle / 2.0, Vector3f::UnitX());
  } else if (activeAxis == 1) {
    rotation = AngleAxisf(0.01 * M_PI, Vector3f::UnitY());
  } else if (activeAxis == 2) {
    rotation = AngleAxisf(0.01 * M_PI, Vector3f::UnitZ());
  }

  setOrientation(currentRotation_WF * rotation);
  callback(currentTransform_WF);
  return true;
}

void RotateControl::setPosition(const Vector3f& newPos) {
  currentTransform_WF.translation() = newPos;
  transform_Wx.translation() = newPos;
  transform_Wy.translation() = newPos;
  transform_Wz.translation() = newPos;
}

void RotateControl::setOrientation(const Quaternionf& orientation) {
  auto t = Translation3f(currentTransform_WF.translation());
  currentTransform_WF = t * orientation;
  transform_Wx = t * orientation * rotation_Fx;
  transform_Wy = t * orientation * rotation_Fy;
  transform_Wz = t * orientation * rotation_Fz;
}

void RotateControl::render(const Camera& camera) const {
  bgfx::setUniform(u_lightDir, lightDir.data(), 1);
  bgfx::setUniform(u_color, xDiskColor.data(), 1);
  bgfx::setTransform(transform_Wx.matrix().data(), 1);
  diskDrawable->setDrawingGeometry();
  bgfx::submit(viewId, program);

  bgfx::setUniform(u_lightDir, lightDir.data(), 1);
  bgfx::setUniform(u_color, yDiskColor.data(), 1);
  bgfx::setTransform(transform_Wy.matrix().data(), 1);
  diskDrawable->setDrawingGeometry();
  bgfx::submit(viewId, program);

  bgfx::setUniform(u_lightDir, lightDir.data(), 1);
  bgfx::setUniform(u_color, zDiskColor.data(), 1);
  bgfx::setTransform(transform_Wz.matrix().data(), 1);
  diskDrawable->setDrawingGeometry();
  bgfx::submit(viewId, program);
}

} // namespace views::controls
