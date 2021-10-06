#pragma once
#include "view_context_3d.h"
#include "views/view.h"

namespace controllers {
class Controller {
protected:
  views::Rect rect;
public:
  /*
   * A view controller manages zero or more views.
   * It manages a rectangular area as defined by the rect.
   * There can be several view controllers stacked on top of each other.
   * Controllers are then managed in a hierarchy, where the parents
   * are free to choose how to route and handle events.
   */
  virtual void viewWillAppear(int width, int height){};
  void setRect(const views::Rect& newRect) {
    rect = newRect;
  };

  /*
   * Event handlers return false, if the event was not handled and the
   * controller deems that the event should be propagated. Respecting this is
   * up to the parent controller that passed the event to this controller.
   * Returning true, means that the event was handled and that it likely
   * should not be propagated. Again, it is up to the parent controller
   * to decide how these should be routed.
   */
  virtual bool leftButtonDown(const ViewContext3D& viewContext) { return false; };
  virtual bool leftButtonUp(const ViewContext3D& viewContext) { return false; };
  virtual bool mouseMoved(const ViewContext3D& viewContext) { return false; };
  virtual bool scroll(double xoffset, double yoffset) { return false; };
  virtual bool keypress(char character) { return false; };
};
} // namespace controllers
