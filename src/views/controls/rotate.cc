#include <bx/math.h>
#include "views/controls/rotate.h"
#include "views/view.h"
#include "geometry/mesh.h"
#include "views/mesh_view.h"
#include "shader_utils.h"
#include "views/controls/control.h"
#include "shader_utils.h"
#include "geometry/mesh.h"
#include "asset_utils.h"
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

  std::filesystem::path assetDir = asset_utils::findAssetDirectory();
  auto diskMesh = std::make_shared<geometry::Mesh>((assetDir / "disk.ply").string(), Matrix4f::Identity(), 0.001);
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
  const Vector3f& cameraOrigin_W = (viewContext.camera.getPosition() - currentTransform_WF.translation());
  const Vector3f& rayDirection_W = viewContext.camera.computeRayWorld(viewContext.width, viewContext.height,
                                                                      viewContext.mousePositionX, viewContext.mousePositionY);
  const Vector3f& cameraOrigin_F = currentTransform_WF.rotation().transpose() * cameraOrigin_W;
  const Vector3f& rayDirection_F = currentTransform_WF.rotation().transpose() * rayDirection_W;

  auto hitX = rtDiskMesh->traceRay(transform_Wx.rotation().transpose() * cameraOrigin_W, transform_Wx.rotation().transpose() * rayDirection_W);
  auto hitY = rtDiskMesh->traceRay(transform_Wy.rotation().transpose() * cameraOrigin_W, transform_Wy.rotation().transpose() * rayDirection_W);
  auto hitZ = rtDiskMesh->traceRay(transform_Wz.rotation().transpose() * cameraOrigin_W, transform_Wz.rotation().transpose() * rayDirection_W);
  float smallestDist = std::numeric_limits<float>::max();
  if (hitX.has_value()) {
    smallestDist = -cameraOrigin_F[0] / rayDirection_F[0];
    activeAxis = 0;
    rotationAxis = Vector3f::UnitX();
  }

  if (hitY.has_value()) {
    float dist = -cameraOrigin_F[1] / rayDirection_F[1];
    if (dist < smallestDist) {
      activeAxis = 1;
      smallestDist = dist;
      rotationAxis = Vector3f::UnitY();
    }
  }

  if (hitZ.has_value()) {
    float dist = -cameraOrigin_F[2] / rayDirection_F[2];
    if (dist < smallestDist) {
      activeAxis = 2;
      smallestDist = dist;
      rotationAxis = Vector3f::UnitZ();
    }
  }

  if (activeAxis != -1) {
    float t = -cameraOrigin_F[activeAxis] / rayDirection_F[activeAxis];
    dragPoint = cameraOrigin_F + t * rayDirection_F;
    return true;
  } else {
    return false;
  }
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
  Quaternionf rotation;
  Quaternionf currentRotation_WF;

  currentRotation_WF = currentTransform_WF.rotation();

  float t = -cameraOrigin_F[activeAxis] / rayDirection_F[activeAxis];
  Vector3f newDragPoint = cameraOrigin_F + t * rayDirection_F;

  auto angle = std::acos(newDragPoint.normalized().dot(dragPoint.normalized()));

  if (std::isnan(angle) || std::abs(angle) > 0.5) {
    angle = 0.0;
  }

  int dragDirection;
  Vector3f dragCrossProduct = dragPoint.cross(newDragPoint);
  if (dragCrossProduct.dot(rotationAxis) < 0.0) {
    dragDirection = -1;
  } else {
    dragDirection = 1;
  }

  rotation = AngleAxisf(dragDirection * angle / 1.5, rotationAxis);

  setOrientation(currentRotation_WF * rotation);
  callback(currentTransform_WF);

  dragPoint = newDragPoint;
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
