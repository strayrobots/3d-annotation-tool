#pragma once
#include "view_context_3d.h"
#include "id.h"
#include "input.h"

namespace views {

using namespace Eigen;

class Rect {
public:
  float x;
  float y;
  float width;
  float height;
  bool hit(const ViewContext3D& ctx) const {
    if (ctx.mousePositionX > x &&
        ctx.mousePositionX < x + width &&
        ctx.mousePositionY > y &&
        ctx.mousePositionY < y + height) {
      return true;
    }
    return false;
  }
};

class View {
protected:
  /*
   * A view is a rectangular region on the screen.
   * It has a size and position as defined by the rect.
   * A view is expected to be managed by a view controller.
   * There may be several views stacked on top of each other and the precedence
   * of event handling is up to the view controller to determine.
   */
  Rect rect;
public:
  int viewId;

  View(int id = -1, const Rect r = {0.0, 0.0, 0.0, 0.0}) : rect(r) {
    if (id == -1) {
      viewId = IdFactory::getInstance().getId();
    } else {
      viewId = id;
    }
    setRect(r);
  }
  virtual ~View(){};

  void setRect(const Rect newRect) {
    rect = newRect;
  };

  bool hit(const ViewContext3D& ctx) const;
};

class View3D {
public:
  int viewId;
  /*
   * A 3D view is slightly more abstract and can take any shape or form.
   * It can be an element inside a 3D view which can take any geometric shape.
   * The exact event handling and collision testing model is still a work in
   * progress and remains to be defined.
   */
  View3D(int viewId = -1) : viewId(viewId){};
  virtual ~View3D(){};
  virtual bool leftButtonDown(const ViewContext3D& viewContext) { return false; }
  virtual bool leftButtonUp(const ViewContext3D& viewContext) { return false; }
  virtual bool mouseMoved(const ViewContext3D& viewContext) { return false; }
  virtual bool keypress(const char character, const InputModifier& mod) { return false; };
  void setCameraTransform(const ViewContext3D& context) const;
};

} // namespace views
