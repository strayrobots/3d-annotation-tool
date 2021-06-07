#pragma once
#include <memory>
#include "views/view.h"
#include "scene_model.h"
#include "view_context_3d.h"
#include "scene_model.h"
#include "timeline.h"

namespace views {

using namespace commands;

class AddKeypointView : public views::View3D {
private:
  // 3D point on mesh under the cursor.
  std::optional<Vector3f> pointingAt;

  SceneModel& sceneModel;
  Timeline& timeline;

public:
  AddKeypointView(SceneModel& model, Timeline& timeline, int viewId = 0);

  bool leftButtonUp(const ViewContext3D& viewContext) override;
  bool mouseMoved(const ViewContext3D& viewContext) override;
};
} // namespace views
