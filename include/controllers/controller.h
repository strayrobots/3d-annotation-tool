#pragma once
#include <list>
#include <memory>
#include "view_context_3d.h"
#include "views/view.h"
#include "input.h"

namespace controllers {
class Controller {
protected:
  views::Rect rect;
  /*
   * Controllers should register any subcontrollers they manage here.
   * Event handling is not yet done by hierarchically walking down the tree,
   * but it should be in the future.
   */
  std::list<std::shared_ptr<Controller>> subControllers;
  void addSubController(std::shared_ptr<Controller> controller) { subControllers.push_back(controller); };
public:
  virtual ~Controller() {};
  /*
   * A view controller manages zero or more views.
   * It manages a rectangular area as defined by the rect.
   * There can be several view controllers stacked on top of each other.
   * Controllers are then managed in a hierarchy, where the parents
   * are free to choose how to route and handle events.
   */
  virtual void viewWillAppear(const views::Rect& r) { setRect(r); };
  /*
   * Should be called on resize and on appear. Basically, whenever
   * the rect changes.
   */
  void setRect(const views::Rect& newRect) {
    rect = newRect;
  };
  virtual bool keypress(char character, const InputModifier mod) {
    for (auto& controller : subControllers) {
      if (controller->keypress(character, mod)) {
        return true;
      }
    }
    return false;
  };
  virtual void resize(const views::Rect& r) { setRect(r); };
};

class Controller3D : public Controller {
public:
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
};
} // namespace controllers
