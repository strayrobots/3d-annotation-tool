#pragma once
#include "views/view.h"
#include "view_context_3d.h"

namespace views::controls {
class Control : public View3D {
public:
  virtual bool leftButtonDown(const ViewContext3D& viewContext) { return false; };
  virtual bool leftButtonUp(const ViewContext3D& viewContext) { return false; };
  virtual bool mouseMoved(const ViewContext3D& viewContext) { return false; };
};
} // namespace views::controls
