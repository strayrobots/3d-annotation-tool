#include <vector>
#include <algorithm>
#include <iostream>
#include "controllers/annotation_controller.h"

const Vector4f KeypointColor(1.0, 0.5, 0.5, 1.0);

namespace controllers {

AnnotationController::AnnotationController(const SceneModel& model) : sceneModel(model),
                                                                      controls(), keypoints() {
}

void AnnotationController::viewWillAppear(int width, int height) {
  meshView = std::make_shared<views::MeshView>(width, height);
}

void AnnotationController::addControl(std::shared_ptr<views::controls::Control> c) {
  controls.push_back(c);
}

void AnnotationController::removeControl(std::shared_ptr<views::controls::Control> c) {
  auto found = std::find(controls.begin(), controls.end(), c);
  if (found != controls.end()) {
    controls.erase(found);
  }
}

void AnnotationController::setKeypointPosition(const Keypoint& kp, Vector3f p) {
  auto found = keypoints.find(kp.id);
  if (found != keypoints.end()) {
    found->second.keypointMesh->setPosition(p);
  }
}

// Mouse events.
bool AnnotationController::leftButtonDown(const ViewContext3D& viewContext) {
  return false;
}

bool AnnotationController::leftButtonUp(const ViewContext3D& viewContext) {
  return false;
}

bool AnnotationController::mouseMoved(const ViewContext3D& viewContext) {
  return false;
}

void AnnotationController::addKeypoint(const Keypoint& keypoint) {
  Matrix4f T = Matrix4f::Identity();
  T.block(0, 3, 3, 1) = keypoint.position;
  auto sphere = std::make_shared<geometry::Sphere>(T, 0.01);
  auto sphereDrawable = std::make_shared<views::MeshDrawable>(sphere, KeypointColor);
  KeypointAnnotation annotation(keypoint, sphereDrawable);
  keypoints.insert(std::make_pair(keypoint.id, annotation));
}

void AnnotationController::removeKeypoint(const Keypoint& keypoint) {
  keypoints.erase(keypoint.id);
}

void AnnotationController::render(const Camera& camera) const {
  if (meshView == nullptr) return;
  if (!keypoints.empty()) {
    meshView->setCameraTransform(camera);
    for (auto iter = keypoints.begin(); iter != keypoints.end(); iter++) {
      meshView->renderObject(iter->second.keypointMesh);
    }
  }
  for (auto& control : controls) {
    control->render(camera);
  }
}

} // namespace controllers
