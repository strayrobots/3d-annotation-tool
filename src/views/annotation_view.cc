#include "views/annotation_view.h"
#include "shader_utils.h"
#include "colors.h"

namespace views {

AnnotationView::AnnotationView(const SceneModel& model, int id) : View3D(id), sceneModel(model),
                                                            sphereDrawable(std::make_shared<geometry::Sphere>(Matrix4f::Identity(), 0.01), viewId),
                                                            bboxView(viewId) {}

void AnnotationView::render(const ViewContext3D& context) const {
  const auto keypoints = sceneModel.getKeypoints();
  if (!keypoints.empty()) {
    for (const auto& keypoint : keypoints) {
      Matrix4f T = Matrix4f::Identity();
      T.block<3, 1>(0, 3) = keypoint.position;
      const Vector4f& color = colors::instanceColors[keypoint.instanceId % 10];
      sphereDrawable.render(context, T, color);
    }
  }
  for (auto& bbox : sceneModel.getBoundingBoxes()) {
    setCameraTransform(context);
    bboxView.render(bbox);
  }
}
} // namespace views
