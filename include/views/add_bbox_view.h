#pragma once
#include <iostream>
#include "views/view.h"
#include "views/bbox.h"
#include "scene_model.h"
#include "view_context_3d.h"
#include "timeline.h"

namespace views {

using namespace geometry;

class AddBBoxView : public views::View3D {
private:
  SceneModel& sceneModel;
  Timeline& timeline;
  BBoxView bboxView;
public:
  AddBBoxView(SceneModel& model, Timeline& timeline) : sceneModel(model), timeline(timeline),
    bboxView() {
  }
  bool leftButtonUp(const ViewContext3D& viewContext) override {
    const Vector3f& rayDirection = viewContext.camera.computeRayWorld(viewContext.width, viewContext.height,
                                                                      viewContext.mousePositionX, viewContext.mousePositionY);
    Intersection intersection = sceneModel.traceRayIntersection(viewContext.camera.getPosition(), rayDirection);
    if (intersection.hit) {
      BBox bbox = { .id = -1,
        .position = intersection.point,
        .orientation = Quaternionf::FromTwoVectors(Vector3f::UnitZ(), intersection.normal),
        .dimensions = Vector3f(0.2, 0.2, 0.2)
      };
      sceneModel.addBBox(bbox);
      std::cout << "Added bounding box" << std::endl;
      return true;
    } else {
      return false;
    }
  }

  void render(const ViewContext3D& context) const {
    for (auto& bbox : sceneModel.getBoundingBoxes()) {
      views::setCameraTransform(context);
      bboxView.render(bbox);
    }
  }
};
}
