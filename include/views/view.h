#pragma once
#include "view_context_3d.h"

namespace views {

using namespace Eigen;

void setCameraTransform(const ViewContext3D& context);

class View {
public:
  View() {}
};

class View3D : View {
public:
  virtual bool leftButtonDown(const ViewContext3D& viewContext) { return false; }
  virtual bool leftButtonUp(const ViewContext3D& viewContext) { return false; }
  virtual bool mouseMoved(const ViewContext3D& viewContext) { return false; }

};

} // namespace views

