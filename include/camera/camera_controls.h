#pragma once
#include "view_context_3d.h"

namespace camera {
class CameraControls {
private:
  bool dragging = false, moved = false;
  double sceneScale = 1.0;
  double prevX, prevY;
public:
  CameraControls();
  void setSceneScale(double scale);
  /*
   * Returns true if the user is currently dragging and moving the camera.
   */
  bool active() const;
  bool leftButtonDown(ViewContext3D& viewContext);
  bool leftButtonUp(ViewContext3D& viewContext);
  bool mouseMoved(ViewContext3D& viewContext);
  bool scroll(double xoffset, double yoffset, ViewContext3D& viewContext);
};
}
