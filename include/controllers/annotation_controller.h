#pragma once
#include <vector>
#include <algorithm>
#include <memory>
#include <map>
#include "controllers/controller.h"
#include "views/mesh_view.h"
#include "views/controls/translate.h"
#include "geometry/ray_trace_mesh.h"
#include "scene_model.h"

namespace controllers {
class AnnotationController : public Controller {
private:
  struct KeypointAnnotation {
    Keypoint keypoint;
    std::shared_ptr<views::MeshDrawable> keypointMesh;
    KeypointAnnotation(Keypoint kp, std::shared_ptr<views::MeshDrawable> mesh) : keypoint(kp), keypointMesh(mesh) {}
  };

  const SceneModel& sceneModel;
  std::vector<std::shared_ptr<views::controls::Control>> controls;
  std::map<int, KeypointAnnotation> keypoints;

public:
  std::shared_ptr<views::MeshView> meshView;

  AnnotationController(const SceneModel& sceneModel);
  void viewWillAppear(int width, int height) override;
  void addControl(std::shared_ptr<views::controls::Control> c);
  void removeControl(std::shared_ptr<views::controls::Control> control);

  // Mouse events.
  bool leftButtonDown(const ViewContext3D& viewContext) override;
  bool leftButtonUp(const ViewContext3D& viewContext) override;
  bool mouseMoved(const ViewContext3D& viewContext) override;
  void addKeypoint(const Keypoint& kp);
  void removeKeypoint(const Keypoint& p);
  void render(const Camera& camera) const;

  void setKeypointPosition(const Keypoint& kp, Vector3f p);
};
} // namespace controllers
