#pragma once
#include "views/view.h"
#include "view_context_3d.h"

namespace views::controls {
class Control : public View {
  virtual bool hitTest(const ViewContext3D& viewContext) const = 0;
};
}

