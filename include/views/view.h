#pragma once
#include "view_context_3d.h"

namespace views {

using namespace Eigen;


class View {
protected:
  int viewId = 0;
public:
  View(int viewId = 0) : viewId(viewId) {}
  virtual ~View() {};
  void setCameraTransform(const ViewContext3D& context) const;
};

class View3D : public View {
public:
  View3D(int viewId=0) : View(viewId) {};
  virtual bool leftButtonDown(const ViewContext3D& viewContext) { return false; }
  virtual bool leftButtonUp(const ViewContext3D& viewContext) { return false; }
  virtual bool mouseMoved(const ViewContext3D& viewContext) { return false; }
};

} // namespace views
