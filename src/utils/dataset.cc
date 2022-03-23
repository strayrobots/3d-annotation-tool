#include "utils/dataset.h"
#include "utils/serialize.h"
#include <iostream>
#include <fstream>
#include "3rdparty/json.hpp"
#include "commands/keypoints.h"
#include "commands/bounding_box.h"
#include "commands/rectangle.h"
#include <memory>

namespace fs = std::filesystem;

using namespace commands;
using json = nlohmann::json;

namespace utils::dataset {

fs::path getAnnotationPathForPointCloudPath(const fs::path& path) {
  fs::path annotationPath(path);
  return annotationPath.replace_extension(".json");
}

std::vector<fs::path> getDatasetImagePaths(fs::path path) {
  std::vector<fs::path> colorImages;
  for (auto& p : fs::directory_iterator(path)) {
    // Skip if dotfile.
    if (p.path().filename().string().at(0) == '.') {
      continue;
    }
    colorImages.push_back(p.path());
  }
  std::sort(colorImages.begin(), colorImages.end());
  return colorImages;
}

std::vector<fs::path> getDatasetPointCloudPaths(fs::path path) {
  std::vector<fs::path> pointClouds;
  for (auto& p : fs::directory_iterator(path)) {
    // Skip if dotfile.
    if ((p.path().filename().string().at(0) == '.') || !(p.path().filename().string().substr(p.path().filename().string().find_last_of(".") + 1) == "ply")) {
      continue;
    }
    pointClouds.push_back(p.path());
  }
  std::sort(pointClouds.begin(), pointClouds.end());
  return pointClouds;
}

std::vector<Eigen::Matrix4f> getDatasetCameraTrajectory(fs::path path) {
  if (!std::filesystem::exists(path)) {
    std::cout << "Camera trajectory does not exist at " << path.string() << std::endl;
    exit(1);
  }
  std::fstream in;
  in.open(path.string(), std::ios::in);
  if (in.fail()) return {};
  std::string line;
  Eigen::Vector4f row;
  std::vector<Eigen::Matrix4f> out;
  while (true) {
    if (!std::getline(in, line)) {
      break;
    }
    Eigen::Matrix4f pose;
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

DatasetMetadata getDatasetMetadata(fs::path path) {
  DatasetMetadata datasetMetadata;
  if (fs::exists(path)) {
    std::ifstream file(path.string());
    json jsonData;
    file >> jsonData;
    datasetMetadata.numClasses = jsonData.contains("num_classes") ? jsonData["num_classes"].get<int>() : 10;
    for (auto& instance : jsonData["instances"]) {
      int classId = instance["class_id"].get<int>();
      InstanceMetadata instanceMetadata;
      if (instance.contains("name")) {
        instanceMetadata.name = instance["name"].get<std::string>();
      } else {
        std::stringstream stream;
        stream << "Instance of class " << classId;
        instanceMetadata.name = stream.str();
      }
      if (instance.contains("size")) {
        Vector3f size(instance["size"][0].get<float>(), instance["size"][1].get<float>(), instance["size"][2].get<float>());
        instanceMetadata.size = size;
      }
      datasetMetadata.instanceMetadata[classId] = instanceMetadata;
    }
    for (int i = 0; i < datasetMetadata.numClasses; i++) {
      if (!datasetMetadata.instanceMetadata.contains(i)) {
        std::stringstream stream;
        stream << "Instance of class " << i;
        datasetMetadata.instanceMetadata[i] = {.name = stream.str()};
      }
    }
  }
  return datasetMetadata;
}

}; // namespace utils::dataset