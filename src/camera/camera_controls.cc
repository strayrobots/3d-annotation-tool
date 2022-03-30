#include "camera/camera_controls.h"

namespace camera {
CameraControls::CameraControls() {}

void CameraControls::setSceneScale(double scale) {
  sceneScale = scale;
}

bool CameraControls::active() {
  return dragging;
}

bool CameraControls::leftButtonDown(ViewContext3D& viewContext) {
  dragging = true;
  moved = false;
  prevX = viewContext.mousePositionX;
  prevY = viewContext.mousePositionY;
  return true;
}

bool CameraControls::leftButtonUp(ViewContext3D& viewContext) {
  if (moved) {
    moved = false;
    dragging = false;
    return true;
  }
  moved = false;
  dragging = false;
  return false;
}

bool CameraControls::mouseMoved(ViewContext3D& viewContext) {
  if (dragging) {
    moved = true;
    float diffX = float(viewContext.mousePositionX - prevX); // TODO: scale according to cloud size
    float diffY = float(viewContext.mousePositionY - prevY); // TODO: scale according to cloud size
    if (viewContext.modifiers & ModCommand) {
      viewContext.camera.translate(Vector3f(-diffX / float(viewContext.width), diffY / float(viewContext.height), 0));
    } else {
      Quaternionf q = AngleAxisf(diffX * M_PI / 2000, Vector3f::UnitY()) * AngleAxisf(diffY * M_PI / 2000, Vector3f::UnitX());
      viewContext.camera.rotateAroundTarget(q);
    }
    prevX = viewContext.mousePositionX;
    prevY = viewContext.mousePositionY;
  } else {
    return false;
  }

  return true;
}

bool CameraControls::scroll(double xoffset, double yoffset, ViewContext3D& viewContext) {
  float diff = yoffset * sceneScale / 5;
  if (viewContext.modifiers & ModCommand) {
    Vector3f fwd(0.0, 0.0, 1.0);
    viewContext.camera.translate(fwd * diff);
  } else {
    viewContext.camera.zoom(diff);
  }
  return true;
}
}

