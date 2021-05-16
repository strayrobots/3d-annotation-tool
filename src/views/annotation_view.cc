#include "views/annotation_view.h"
#include "shader_utils.h"
#include <iostream>

namespace views {

const Vector4f KeypointColor(1.0, 0.5, 0.5, 1.0);

AnnotationView::AnnotationView(SceneModel& model, int viewId) : View3D(viewId), sceneModel(model),
    sphereDrawable(std::make_shared<geometry::Sphere>(Matrix4f::Identity(), 0.01), viewId),
    bboxView(viewId)  {
}

void AnnotationView::render(const ViewContext3D& context) const {
  const auto keypoints = sceneModel.getKeypoints();
  if (!keypoints.empty()) {
    for (const auto& keypoint : keypoints) {
      Matrix4f T = Matrix4f::Identity();
      T.block<3, 1>(0, 3) = keypoint.position;
      sphereDrawable.render(context, T, KeypointColor);
    }
  }
  for (auto& bbox : sceneModel.getBoundingBoxes()) {
    setCameraTransform(context);
    bboxView.render(bbox);
  }
}
}
