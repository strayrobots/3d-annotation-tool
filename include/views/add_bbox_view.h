#pragma once
#include <iostream>
#include <memory>
#include "views/view.h"
#include "views/controls/translate.h"
#include "views/controls/rotate.h"
#include "scene_model.h"
#include "view_context_3d.h"
#include "timeline.h"

namespace views {

using namespace geometry;

class AddBBoxView : public views::View3D {
private:
  SceneModel& sceneModel;
  Timeline& timeline;
  std::shared_ptr<views::controls::TranslateControl> translateControl;
  std::shared_ptr<views::controls::RotateControl> rotateControl;
  std::shared_ptr<views::controls::TranslateControl> sizeControl;
  BBox bboxStart = {.id = -1};
  Vector3f position = Vector3f::Zero();
  Quaternionf orientation = Quaternionf::Identity();
  Vector3f dimensions = Vector3f::Zero();
public:
  AddBBoxView(SceneModel& model, Timeline& timeline, int viewId = 0);
  void refresh();
  bool leftButtonUp(const ViewContext3D& viewContext) override;
  bool leftButtonDown(const ViewContext3D& context) override;
  bool mouseMoved(const ViewContext3D& context) override;
  void render(const ViewContext3D& context) const;
};
} // namespace views
