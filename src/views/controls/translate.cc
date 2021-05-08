#include <bx/math.h>
#include "views/controls/translate.h"
#include "views/view.h"
#include "geometry/mesh.h"
#include "views/mesh_view.h"
#include "shader_utils.h"
#include "views/controls/control.h"
#include "shader_utils.h"
#include "geometry/mesh.h"

namespace views::controls {

TranslateControl::TranslateControl(std::function<void(const Vector3f&)> cb) : callback(cb) {
  yRotation = AngleAxisf(M_PI / 2.0, Vector3f(0.0, 0.0, 1.0));
  zRotation = AngleAxisf(-M_PI / 2.0, Vector3f(0.0, 1.0, 0.0));
  yTransform = Transform<float, 3, Eigen::Affine>::Identity();
  zTransform = Transform<float, 3, Eigen::Affine>::Identity();
  yTransform.rotate(yRotation);
  zTransform.rotate(zRotation);

  auto xAxisMesh = std::make_shared<geometry::Mesh>("../assets/x_axis.ply", Matrix4f::Identity(), 0.5);
  xAxisDrawable = std::make_shared<views::MeshDrawable>(xAxisMesh);
  rtAxisMesh = std::make_unique<geometry::RayTraceMesh>(xAxisMesh);

  u_lightDir = bgfx::createUniform("u_light_dir", bgfx::UniformType::Vec4);
  u_color = bgfx::createUniform("u_color", bgfx::UniformType::Vec4);
  program = shader_utils::loadProgram("vs_mesh", "fs_mesh_uniform");
}

TranslateControl::~TranslateControl() {
  bgfx::destroy(program);
  bgfx::destroy(u_lightDir);
  bgfx::destroy(u_color);
}

bool TranslateControl::leftButtonDown(const ViewContext3D& viewContext) {
  activeAxis = -1;
  const Vector3f& cameraOrigin = (viewContext.camera.getPosition() - currentTransform.translation());
  const Vector3f& rayDirection = viewContext.camera.computeRayWorld(viewContext.width, viewContext.height,
                                                                    viewContext.mousePositionX, viewContext.mousePositionY);
  auto hitX = rtAxisMesh->traceRay(currentTransform.rotation().transpose() * cameraOrigin, currentTransform.rotation().transpose() * rayDirection);
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

bool TranslateControl::leftButtonUp(const ViewContext3D& viewContext) {
  bool wasActive = activeAxis != -1;
  activeAxis = -1;
  return wasActive;
}

bool TranslateControl::mouseMoved(const ViewContext3D& viewContext) {
  if (activeAxis < 0) return false;
  const Vector3f& cameraOrigin = currentTransform.rotation().transpose() * (viewContext.camera.getPosition() - currentTransform.translation());
  const Vector3f& rayDirection = currentTransform.rotation().transpose() * viewContext.camera.computeRayWorld(viewContext.width, viewContext.height,
                                                                    viewContext.mousePositionX, viewContext.mousePositionY);
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
  currentTransform.translation() += currentTransform.rotation() * change;
  yTransform.translation() = currentTransform.translation();
  zTransform.translation() = currentTransform.translation();
  callback(currentTransform.translation());
  return true;
}

void TranslateControl::setPosition(const Vector3f& newPos) {
  currentTransform.translation() = newPos;
  yTransform.translation() = newPos;
  zTransform.translation() = newPos;
}

void TranslateControl::setOrientation(const Quaternionf& orientation) {
  auto t = Translation3f(currentTransform.translation());
  currentTransform = t * orientation;
  yTransform = t * orientation * yRotation;
  zTransform = t * orientation * zRotation;
}

void TranslateControl::render(const Camera& camera) const {
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

} // namespace views::controls
