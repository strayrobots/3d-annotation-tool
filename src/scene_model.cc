#include <fstream>
#include <iostream>
#include <algorithm>
#include <sstream>
#include "scene_model.h"
#include "3rdparty/json.hpp"

namespace fs = std::filesystem;
using json = nlohmann::json;

SceneModel::SceneModel(const std::string& datasetFolder, bool rayTracing) : datasetPath(datasetFolder),
                                                                            keypoints(), boundingBoxes() {
  loadCameraParams();
  loadSceneMetadata();
  if (rayTracing) {
    initRayTracing();
  }
}

std::shared_ptr<geometry::TriangleMesh> SceneModel::getMesh() const { return mesh; }

std::optional<Vector3f> SceneModel::traceRay(const Vector3f& origin, const Vector3f& direction) {
  if (!rtMesh.has_value()) return {};
  return rtMesh->traceRay(origin, direction);
}

geometry::Intersection SceneModel::traceRayIntersection(const Vector3f& origin, const Vector3f& direction) {
  if (!rtMesh.has_value()) return {};
  return rtMesh->traceRayIntersection(origin, direction);
}

Keypoint SceneModel::addKeypoint(const Vector3f& position) {
  Keypoint keypoint(keypoints.size() + 1, currentInstanceId, position);
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

std::optional<Keypoint> SceneModel::getKeypoint(int id) const {
  for (unsigned int i = 0; i < keypoints.size(); i++) {
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

std::vector<fs::path> SceneModel::imagePaths() const {
  std::vector<fs::path> colorImages;
  auto colorDir = datasetPath / "color";
  for (auto& p : fs::directory_iterator(colorDir)) {
    // Skip if dotfile.
    if (p.path().filename().string().at(0) == '.') {
      continue;
    }
    colorImages.push_back(p.path());
  }
  std::sort(colorImages.begin(), colorImages.end());
  return colorImages;
}

void SceneModel::addBoundingBox(BBox& bbox) {
  bbox.id = boundingBoxes.size() + 1;
  boundingBoxes.push_back(bbox);
}


template<class T>
void removeAnnotation(std::vector<T>& entities, int id) {
  if (entities.empty()) return;
  auto iterator = std::find_if(entities.begin(), entities.end(), [&](const T& annotation) {
    return annotation.id == id;
  });
  if (iterator != entities.end()) {
    entities.erase(iterator);
  }
}

void SceneModel::removeBoundingBox(int id) {
  removeAnnotation(boundingBoxes, id);
}

void SceneModel::updateBoundingBox(const BBox& updated) {
  auto iterator = std::find_if(boundingBoxes.begin(), boundingBoxes.end(), [&](const BBox& bbox) {
    return bbox.id == updated.id;
  });

  if (iterator != boundingBoxes.end()) {
    *iterator = updated;
  }
}

void SceneModel::addRectangle(Rectangle& rectangle) {
  rectangle.id = rectangles.size() + 1;
  rectangles.push_back(rectangle);
}

void SceneModel::removeRectangle(int id) {
  removeAnnotation(rectangles, id);
}

Camera SceneModel::sceneCamera() const {
  return Camera(cameraMatrix, imageHeight);
}

std::pair<int, int> SceneModel::imageSize() const {
  return std::make_pair(imageWidth, imageHeight);
}

std::vector<Matrix4f> SceneModel::cameraTrajectory() const {
  auto trajectoryLogPath = datasetPath / "scene" / "trajectory.log";
  if (!std::filesystem::exists(trajectoryLogPath)) {
    std::cout << "Camera trajectory does not exist at " << trajectoryLogPath.string() << std::endl;
    exit(1);
  }
  std::fstream in;
  in.open(trajectoryLogPath.string(), std::ios::in);
  if (in.fail()) return {};
  std::string line;
  Vector4f row;
  std::vector<Matrix4f> out;
  while (true) {
    if (!std::getline(in, line)) {
      break;
    }
    Matrix4f pose;
    for (int i = 0; i < 4; i++) {
      std::getline(in, line);
      std::stringstream lineStream(line);
      for (int j = 0; j < 4; j++) {
        lineStream >> row[j];
      }
      pose.row(i) = row;
    }
    out.push_back(pose);
  }
  return out;
}

nlohmann::json serialize(const Vector3f& v) {
  auto out = nlohmann::json::array();
  out[0] = v[0];
  out[1] = v[1];
  out[2] = v[2];
  return out;
}

nlohmann::json serialize(const Vector2f& v) {
  auto out = nlohmann::json::array();
  out[0] = v[0];
  out[1] = v[1];
  return out;
}

nlohmann::json serialize(const Keypoint& keypoint) {
  auto out = nlohmann::json::object();
  out["instance_id"] = keypoint.instanceId;
  out["position"] = serialize(keypoint.position);
  return out;
}

nlohmann::json serialize(const BBox& bbox) {
  auto obj = nlohmann::json::object();
  obj["position"] = serialize(bbox.position);
  obj["orientation"] = {
      {"w", bbox.orientation.w()},
      {"x", bbox.orientation.x()},
      {"y", bbox.orientation.y()},
      {"z", bbox.orientation.z()}};
  obj["dimensions"] = serialize(bbox.dimensions);
  obj["instance_id"] = bbox.instanceId;
  return obj;
}

nlohmann::json serialize(const Rectangle& rectangle) {
  auto obj = nlohmann::json::object();
  obj["center"] = serialize(rectangle.center);
  obj["orientation"] = {
      {"w", rectangle.orientation.w()},
      {"x", rectangle.orientation.x()},
      {"y", rectangle.orientation.y()},
      {"z", rectangle.orientation.z()}};
  obj["size"] = serialize(rectangle.size);
  obj["class_id"] = rectangle.classId;
  return obj;
}

void SceneModel::save() const {
  auto annotationPath = datasetPath / "annotations.json";
  nlohmann::json json = nlohmann::json::object();
  if (!keypoints.empty()) {
    json["keypoints"] = nlohmann::json::array();
    for (size_t i = 0; i < keypoints.size(); i++) {
      json["keypoints"][i] = serialize(keypoints[i]);
    }
  }
  if (!boundingBoxes.empty()) {
    json["bounding_boxes"] = nlohmann::json::array();
    for (size_t i = 0; i < boundingBoxes.size(); i++) {
      const auto& bbox = boundingBoxes[i];
      json["bounding_boxes"][i] = serialize(bbox);
    }
  }
  if (!rectangles.empty()) {
    json["rectangles"] = nlohmann::json::array();
    for (size_t i=0; i < rectangles.size(); i++) {
      json["rectangles"][i] = serialize(rectangles[i]);
    }
  }
  std::ofstream file(annotationPath.string());
  file << json.dump(4);
  std::cout << "Saved annotations to " << annotationPath.string() << std::endl;
}

void SceneModel::load() {
  auto annotationPath = datasetPath / "annotations.json";

  nlohmann::json json;
  std::ifstream file(annotationPath);
  file >> json;
  for (auto& point : json["keypoints"]) {
    auto position = point["position"];
    auto instanceId = point["instance_id"].get<int>();
    Keypoint kp(keypoints.size() + 1, instanceId, Vector3f(position[0].get<float>(), position[1].get<float>(), position[2].get<float>()));
    keypoints.push_back(kp);
  }
  for (auto& bbox : json["bounding_boxes"]) {
    auto p = bbox["position"];
    auto orn = bbox["orientation"];
    auto d = bbox["dimensions"];
    auto instanceId = bbox["instance_id"];
    BBox box = {
        .id = int(boundingBoxes.size()) + 1,
        .instanceId = instanceId,
        .position = Vector3f(p[0].get<float>(), p[1].get<float>(), p[2].get<float>()),
        .orientation = Quaternionf(orn["w"].get<float>(), orn["x"].get<float>(), orn["y"].get<float>(), orn["z"].get<float>()),
        .dimensions = Vector3f(d[0].get<float>(), d[1].get<float>(), d[2].get<float>())};
    boundingBoxes.push_back(box);
  }
}

void SceneModel::initRayTracing() {
  mesh = std::make_shared<geometry::Mesh>((datasetPath / "scene" / "integrated.ply").string());
  rtMesh.emplace(mesh);
}

void SceneModel::loadCameraParams() {
  auto intrinsicsPath = datasetPath / "camera_intrinsics.json";
  if (!std::filesystem::exists(intrinsicsPath)) {
    std::cout << "camera intrinsics do not exist at " << intrinsicsPath.string() << std::endl;
    exit(1);
  }
  std::ifstream file(intrinsicsPath.string());
  nlohmann::json json;
  file >> json;
  auto matrix = json["intrinsic_matrix"];
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      cameraMatrix(i, j) = matrix[j * 3 + i];
    }
  }
  imageHeight = json["height"];
  imageWidth = json["width"];
}

void SceneModel::loadSceneMetadata() {
  auto metadataPath = datasetPath.parent_path() / "metadata.json";
  if (fs::exists(metadataPath)) {
    std::ifstream file(metadataPath.string());
    json jsonData;
    file >> jsonData;
    datasetMetadata.numClasses = jsonData.contains("num_classes") ? jsonData["num_classes"].get<int>() : 10;
    for (auto& instance : jsonData["instances"]) {
      int instanceId = instance["instance_id"].get<int>();
      InstanceMetadata instanceMetadata;
      if (instance.contains("name")) {
        instanceMetadata.name = instance["name"].get<std::string>();
      } else {
        std::stringstream stream;
        stream << "Instance " << instanceId;
        instanceMetadata.name = stream.str();
      }
      if (instance.contains("size")) {
        Vector3f size(instance["size"][0].get<float>(), instance["size"][1].get<float>(), instance["size"][2].get<float>());
        instanceMetadata.size = size;
      }
      datasetMetadata.instanceMetadata[instanceId] = instanceMetadata;
    }
    for (int i=0; i < datasetMetadata.numClasses; i++) {
      if (!datasetMetadata.instanceMetadata.contains(i)) {
        std::stringstream stream;
        stream << "Instance " << i;
        datasetMetadata.instanceMetadata[i] = { .name = stream.str() };
      }
    }
  }
}

Rectangle::Rectangle(const std::array<Vector3f, 4>& vertices) {
  std::array<Vector3f, 4> copy(vertices);
  Vector3f topLeft = vertices[0];
  std::vector<float> distances;
  std::transform(vertices.begin() + 1, vertices.end(), std::back_inserter(distances), [&](const Vector3f& v) -> float {
    return -(v - topLeft).norm();
  });
  int bottomRightIndex = std::min_element(distances.begin(), distances.end()) - distances.begin() + 1;
  // Move bottom right to last position.
  std::swap(copy[bottomRightIndex], copy[3]);

  Vector3f edge1 = (copy[2] - copy[0]);
  Vector3f edge2 = (copy[1] - copy[0]);
  Vector3f bottomRight = copy[3];

  center = (topLeft + bottomRight) / 2.0f;
  Vector3f normal = edge1.cross(edge2);
  normal = normal / normal.norm();

  Eigen::Matrix3f R_RW;
  R_RW.row(0) = edge1 / edge1.norm();
  R_RW.row(1) = edge2 / edge2.norm();
  R_RW.row(2) = normal;
  orientation = Quaternionf(R_RW);

  id = 0;
  classId = 0;
  size = Vector2f(edge1.norm(), edge2.norm());
}

Rectangle::Rectangle(int id, int classId, Vector3f center, Quaternionf orientation, Vector2f size) :
  id(id), classId(classId), center(center), orientation(orientation), size(size) {}

float Rectangle::width() const { return size[0]; }
float Rectangle::height() const { return size[1]; }

Vector3f Rectangle::normal() const {
  return orientation * Vector3f::UnitZ();
}

