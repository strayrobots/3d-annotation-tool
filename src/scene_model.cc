#include <fstream>
#include <iostream>
#include <algorithm>
#include <sstream>
#include "scene_model.h"
#include "3rdparty/json.hpp"
#include "utils/serialize.h"

namespace fs = std::filesystem;
using json = nlohmann::json;

SceneModel::SceneModel(std::optional<std::string> meshPath) : meshPath(meshPath), keypoints(), boundingBoxes() {}

std::shared_ptr<geometry::TriangleMesh> SceneModel::getMesh() {
  if (mesh == nullptr) {
    loadMesh();
  }
  return mesh;
}

std::shared_ptr<geometry::PointCloud> SceneModel::getPointCloud() {
  if (pointCloud == nullptr) {
    loadPointCloud();
  }
  return pointCloud;
}

std::optional<Vector3f> SceneModel::traceRay(const Vector3f& origin, const Vector3f& direction) {
  if (activeView == active_view::MeshView) {
    if (!rtMesh.has_value()) return {};
    return rtMesh->traceRay(origin, direction);
  } else {
    if (!rtPointCloud.has_value()) return {};
    return rtPointCloud->traceRay(origin, direction);
  }
}

geometry::Intersection SceneModel::traceRayIntersection(const Vector3f& origin, const Vector3f& direction) {
  if (activeView == active_view::MeshView) {
    if (!rtMesh.has_value()) return {};
    return rtMesh->traceRayIntersection(origin, direction);
  } else {
    if (!rtPointCloud.has_value()) return {};
    return rtPointCloud->traceRayIntersection(origin, direction);
  }
}

Keypoint SceneModel::addKeypoint(const Vector3f& position) {
  Keypoint keypoint(keypoints.size() + 1, currentClassId, position);
  keypoints.push_back(keypoint);
  return keypoint;
}
Keypoint SceneModel::addKeypoint(const Keypoint& kp) {
  Keypoint keypoint = kp;
  keypoint.id = keypoints.size() + 1;
  keypoints.push_back(keypoint);
  return keypoint;
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

void SceneModel::reset() {
  keypoints.clear();
  boundingBoxes.clear();
  rectangles.clear();
}

std::optional<Keypoint> SceneModel::getKeypoint(int id) const {
  for (unsigned int i = 0; i < keypoints.size(); i++) {
    if (keypoints[i].id == id) {
      return keypoints[i];
    }
  }
  return {};
}

void SceneModel::setPointCloudPath(std::string path) {
  pointCloud = nullptr;
  pointCloudPath = path;
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
  for (unsigned int i = 0; i < keypoints.size(); i++) {
    if (keypoints[i].id == id) {
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

template <class T>
void removeAnnotation(std::vector<T>& entities, int id) {
  if (entities.empty()) return;
  auto iterator = std::find_if(entities.begin(), entities.end(), [&](const T& annotation) {
    return annotation.id == id;
  });
  if (iterator != entities.end()) {
    entities.erase(iterator);
  }
}
template <class T>
void updateAnnotation(std::vector<T>& annotations, const T& updated) {
  auto iterator = std::find_if(annotations.begin(), annotations.end(), [&](const T& annotation) {
    return annotation.id == updated.id;
  });

  if (iterator != annotations.end()) {
    *iterator = updated;
  } else {
    std::cout << "could not find annotation: " << updated.id << std::endl;
  }
}

void SceneModel::removeBoundingBox(int id) {
  removeAnnotation(boundingBoxes, id);
}

void SceneModel::updateBoundingBox(const BBox& updated) {
  updateAnnotation(boundingBoxes, updated);
}

void SceneModel::addRectangle(Rectangle& rectangle) {
  rectangles.push_back(rectangle);
}

void SceneModel::removeRectangle(int id) {
  removeAnnotation(rectangles, id);
}

void SceneModel::updateRectangle(const Rectangle& updated) {
  updateAnnotation(rectangles, updated);
}

void SceneModel::loadMesh() {
  if (meshPath) {
    mesh = std::make_shared<geometry::Mesh>(meshPath.value_or("empty"));
    rtMesh.emplace(mesh);
  }
}

void SceneModel::loadPointCloud() {
  if (pointCloudPath) {
    pointCloud = std::make_shared<geometry::PointCloud>(pointCloudPath.value_or("empty"));
    rtPointCloud.emplace(pointCloud, pointCloudPointSize);
  }
}

void SceneModel::load(fs::path annotationPath) {
  nlohmann::json json;
  std::ifstream file(annotationPath);
  file >> json;
  for (auto& point : json["keypoints"]) {
    auto position = point["position"];
    auto classId = point["class_id"].get<int>();
    Keypoint kp(keypoints.size() + 1, classId, Vector3f(position[0].get<float>(), position[1].get<float>(), position[2].get<float>()));
    keypoints.push_back(kp);
  }
  for (auto& bbox : json["bounding_boxes"]) {
    auto p = bbox["position"];
    auto orn = bbox["orientation"];
    auto d = bbox["dimensions"];
    auto classId = bbox["class_id"];
    BBox box = {
        .id = int(boundingBoxes.size()) + 1,
        .classId = classId,
        .position = Vector3f(p[0].get<float>(), p[1].get<float>(), p[2].get<float>()),
        .orientation = Quaternionf(orn["w"].get<float>(), orn["x"].get<float>(), orn["y"].get<float>(), orn["z"].get<float>()),
        .dimensions = Vector3f(d[0].get<float>(), d[1].get<float>(), d[2].get<float>())};
    boundingBoxes.push_back(box);
  }

  for (auto& rectangle : json["rectangles"]) {
    Rectangle rect(0, rectangle["class_id"],
                   utils::serialize::toVector3(rectangle["center"]),
                   utils::serialize::toQuaternion(rectangle["orientation"]),
                   utils::serialize::toVector2(rectangle["size"]));
    rectangles.push_back(rect);
  }
}

void SceneModel::save(fs::path annotationPath) const {
  nlohmann::json json = nlohmann::json::object();
  if (!keypoints.empty()) {
    json["keypoints"] = nlohmann::json::array();
    for (size_t i = 0; i < keypoints.size(); i++) {
      json["keypoints"][i] = utils::serialize::serialize(keypoints[i]);
    }
  }
  if (!boundingBoxes.empty()) {
    json["bounding_boxes"] = nlohmann::json::array();
    for (size_t i = 0; i < boundingBoxes.size(); i++) {
      const auto& bbox = boundingBoxes[i];
      json["bounding_boxes"][i] = utils::serialize::serialize(bbox);
    }
  }
  if (!rectangles.empty()) {
    json["rectangles"] = nlohmann::json::array();
    for (size_t i = 0; i < rectangles.size(); i++) {
      json["rectangles"][i] = utils::serialize::serialize(rectangles[i]);
    }
  }
  std::ofstream file(annotationPath.string());
  file << json.dump(4);
  std::cout << "Saved annotations to " << annotationPath.string() << std::endl;
}
