#pragma once
#include "view_context_3d.h"
#include "id.h"
#include "input.h"

namespace views {

using namespace Eigen;

struct Rect {
  float x;
  float y;
  float width;
  float height;
};

class View {
public:
  int viewId;

  View(int id = -1) {
    if (id == -1) {
      viewId = IdFactory::getInstance().getId();
    } else {
      viewId = id;
    }
  }
  virtual ~View(){};
  void setCameraTransform(const ViewContext3D& context) const;
};

class View3D : public View {
public:
  View3D(int viewId = -1) : View(viewId) {};
  virtual bool leftButtonDown(const ViewContext3D& viewContext) { return false; }
  virtual bool leftButtonUp(const ViewContext3D& viewContext) { return false; }
  virtual bool mouseMoved(const ViewContext3D& viewContext) { return false; }
  virtual bool keypress(const char character, const InputModifier& mod) { return false; };
};

} // namespace views
