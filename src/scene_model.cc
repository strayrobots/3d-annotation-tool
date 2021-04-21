#include <fstream>
#include <iostream>
#include "scene_model.h"
#include "3rdparty/json.hpp"

SceneModel::SceneModel(const std::string& datasetFolder) : datasetPath(datasetFolder),
    mesh(new geometry::Mesh((datasetPath / "scene" / "integrated.ply").string())),
    rtMesh(*mesh) {
  initRayTracing();
}

std::shared_ptr<geometry::TriangleMesh> SceneModel::getMesh() const { return mesh; }

std::optional<Vector3f> SceneModel::traceRay(const Vector3f& origin, const Vector3f& direction) {
  return rtMesh.traceRay(origin, direction);
}

void SceneModel::popKeypoint() {
  if (keypoints.empty()) return;
  keypoints.pop_back();
}

void SceneModel::save() const {
  auto keypointPath = datasetPath / "keypoints.json";
  nlohmann::json json = nlohmann::json::array();
  for (size_t i = 0; i < keypoints.size(); i++) {
    json[i] = { {"x", keypoints[i][0]}, {"y", keypoints[i][1]}, {"z", keypoints[i][2]} };
  }
  std::ofstream file(keypointPath.string());
  file << json;
  std::cout << "Saved keypoints to keypoints.json" << std::endl;
}

void SceneModel::initRayTracing() {
}

