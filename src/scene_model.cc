#include <fstream>
#include <iostream>
#include <algorithm>
#include "scene_model.h"
#include "3rdparty/json.hpp"

SceneModel::SceneModel(const std::string& datasetFolder) : datasetPath(datasetFolder),
                                                           mesh(new geometry::Mesh((datasetPath / "scene" / "integrated.ply").string())),
                                                           rtMesh(mesh) {
  initRayTracing();
}

std::shared_ptr<geometry::TriangleMesh> SceneModel::getMesh() const { return mesh; }

std::optional<Vector3f> SceneModel::traceRay(const Vector3f& origin, const Vector3f& direction) {
  return rtMesh.traceRay(origin, direction);
}

geometry::Intersection SceneModel::traceRayIntersection(const Vector3f& origin, const Vector3f& direction) {
  return rtMesh.traceRayIntersection(origin, direction);
}

Keypoint SceneModel::addKeypoint(const Vector3f& p) {
  Keypoint kp(keypoints.size() + 1, p);
  keypoints.push_back(kp);
  return kp;
}

void SceneModel::removeKeypoint(const Keypoint& kp) {
  if (keypoints.empty()) return;
  auto iterator = std::find_if(keypoints.begin(), keypoints.end(), [&](const Keypoint& keypoint) {
    return keypoint.id == kp.id;
  });
  if (iterator == keypoints.end()) {
    std::cout << "Keypoint " << kp.id << " was not found. Should not happen." << std::endl;
    return;
  }
  keypoints.erase(iterator);
}

std::optional<Keypoint> SceneModel::getKeypoint(int id) const {
  for (int i = 0; i < keypoints.size(); i++) {
    if (keypoints[i].id == id) {
      return keypoints[i];
    }
  }
  return {};
}

void SceneModel::setKeypoint(const Keypoint& updated) {
  auto point = std::find_if(keypoints.begin(), keypoints.end(), [&](const Keypoint& keypoint) {
    return keypoint.id == updated.id;
  });
  if (point != keypoints.end()) {
    *point = updated;
  }
}

void SceneModel::updateKeypoint(int id, Keypoint kp) {
  assert(kp.id == id && "Keypoint needs to be the same as the one being updated.");
  int value = -1;
  for (int i = 0; i < keypoints.size(); i++) {
    if (keypoints[i].id == id) {
      value = i;
      keypoints[i] = kp;
      return;
    }
  }
}

// Bounding boxes
void SceneModel::addBBox(BBox& bbox) {
  bbox.id = boundingBoxes.size() + 1;
  boundingBoxes.push_back(bbox);
}

void SceneModel::save() const {
  auto keypointPath = datasetPath / "keypoints.json";
  nlohmann::json json = nlohmann::json::array();
  for (size_t i = 0; i < keypoints.size(); i++) {
    json[i] = {{"x", keypoints[i].position[0]}, {"y", keypoints[i].position[1]}, {"z", keypoints[i].position[2]}};
  }
  std::ofstream file(keypointPath.string());
  file << json;
  std::cout << "Saved keypoints to keypoints.json" << std::endl;
}

void SceneModel::initRayTracing() {
}
