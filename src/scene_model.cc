#include <fstream>
#include <iostream>
#include <algorithm>
#include "scene_model.h"
#include "3rdparty/json.hpp"

SceneModel::SceneModel(const std::string& datasetFolder) : datasetPath(datasetFolder),
                                                           mesh(new geometry::Mesh((datasetPath / "scene" / "integrated.ply").string())),
                                                           rtMesh(mesh), keypoints(), boundingBoxes() {
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
std::optional<BBox> SceneModel::getBoundingBox(int id) const {
  auto iterator = std::find_if(boundingBoxes.begin(), boundingBoxes.end(), [&](const BBox& bbox) {
    return bbox.id == id;
  });
  if (iterator != boundingBoxes.end()) {
    return *iterator;
  }
  return {};
}

void SceneModel::addBoundingBox(BBox& bbox) {
  bbox.id = boundingBoxes.size() + 1;
  boundingBoxes.push_back(bbox);
}

void SceneModel::removeBoundingBox(int id) {
  if (boundingBoxes.empty()) return;
  auto iterator = std::find_if(boundingBoxes.begin(), boundingBoxes.end(), [&](const BBox& bbox) {
    return bbox.id == id;
  });
  if (iterator != boundingBoxes.end()) {
    boundingBoxes.erase(iterator);
  }
}

void SceneModel::updateBoundingBox(const BBox& updated) {
  auto iterator = std::find_if(boundingBoxes.begin(), boundingBoxes.end(), [&](const BBox& bbox) {
    return bbox.id == updated.id;
  });

  if (iterator != boundingBoxes.end()) {
    *iterator = updated;
  }
}

nlohmann::json serializeVector(const Vector3f& v) {
  auto out = nlohmann::json::array();
  out[0] = v[0];
  out[1] = v[1];
  out[2] = v[2];
  return out;
}

nlohmann::json serializeBBox(const BBox& bbox) {
  auto obj = nlohmann::json::object();
  obj["position"] = serializeVector(bbox.position);
  obj["orientation"] = {
    {"w", bbox.orientation.w()},
    {"x", bbox.orientation.x()},
    {"y", bbox.orientation.y()},
    {"z", bbox.orientation.z()}
  };
  obj["dimensions"] = serializeVector(bbox.dimensions);
  return obj;
}

void SceneModel::save() const {
  auto annotationPath = datasetPath / "annotations.json";
  nlohmann::json json = nlohmann::json::object();
  json["keypoints"] = nlohmann::json::array();
  for (size_t i = 0; i < keypoints.size(); i++) {
    json["keypoints"][i] = serializeVector(keypoints[i].position);
  }
  json["bounding_boxes"] = nlohmann::json::array();
  for (size_t i = 0; i < boundingBoxes.size(); i++) {
    const auto& bbox = boundingBoxes[i];
    json["bounding_boxes"][i] = serializeBBox(bbox);
  }
  std::ofstream file(annotationPath.string());
  file << json;
  std::cout << "Saved keypoints to keypoints.json" << std::endl;
}

void SceneModel::initRayTracing() {
}
