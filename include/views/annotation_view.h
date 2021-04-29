#pragma once
#include "views/view.h"
#include "shader_utils.h"
#include "views/controls/translate.h"

namespace views {

const Vector4f KeypointColor(1.0, 0.5, 0.5, 1.0);

class AnnotationView : public views::View3D {
private:

  views::MeshDrawable sphereDrawable;
  SceneModel& sceneModel;

  bgfx::UniformHandle u_color, u_lightDir;
  Vector4f lightDir = Vector4f(0.0, 1.0, -1.0, 1.0);
public:
  AnnotationView(SceneModel& model) : sceneModel(model),
      sphereDrawable(std::make_shared<geometry::Sphere>(Matrix4f::Identity(), 0.01)) {
  }

  void render(const ViewContext3D& context) const {
    const auto keypoints = sceneModel.getKeypoints();
    if (!keypoints.empty()) {
      setCameraTransform(context);
      for (const auto& keypoint : keypoints) {
        Matrix4f T = Matrix4f::Identity();
        T.block<3, 1>(0, 3) = keypoint.position;
        sphereDrawable.render(T, KeypointColor);
      }
    }
  }
};
}
