#include <thread>
#include <chrono>
#include <iostream>
#include <optional>
#include <filesystem>
#include "3rdparty/cxxopts.h"
#include "studio.h"
#include "controllers/studio_view_controller.h"
#include "controllers/point_cloud_view_controller.h"

namespace fs = std::filesystem;

void validateFlags(const cxxopts::ParseResult& flags) {
  bool valid = true;
  if (flags.count("dataset") == 0) {
    std::cout << "Dataset argument is required." << std::endl;
    valid = false;
  } else if (flags.count("dataset") > 1) {
    std::cout << "Only one dataset should be provided." << std::endl;
    valid = false;
  } else if (flags.count("dataset") == 1) {
    std::string dataset = flags["dataset"].as<std::vector<std::string>>()[0];
    if (!std::filesystem::exists(dataset)) {
      std::cout << "Dataset folder does not exist." << std::endl;
      valid = false;
    }
  }
  if (!valid) {
    exit(1);
  }
}

bool isStudioScene(const fs::path& scenePath) {
  if (fs::is_directory(scenePath / "scene") &&
      fs::is_directory(scenePath / "color")) {
    return true;
  }
  return false;
}

bool isPointCloud(const fs::path& pcPath) {
  if (pcPath.extension() == ".ply") {
    return true;
  }
  return false;
}

bool isPointCloudDirectory(const fs::path& pcPath) {
  for (auto& path : fs::directory_iterator(pcPath)) {
    if (fs::exists(path) && fs::path(path).extension() == ".ply") {
      return true;
    }
  }
  return false;
}

std::optional<fs::path> findPointCloud(const fs::path& datasetPath) {
  for (auto& path : fs::directory_iterator(datasetPath)) {
    if (fs::exists(path) && fs::path(path).extension() == ".ply") {
      return path;
    }
  }
  return {};
}

template <class T>
void loop(T& studio) {
  while (studio.update()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
  }
}

int main(int argc, char* argv[]) {
  cxxopts::Options options("Studio", "Annotate the world in 3D.");
  options.add_options()("dataset", "That path to folder of the dataset to annotate.",
                        cxxopts::value<std::vector<std::string>>());
  options.parse_positional({"dataset"});
  cxxopts::ParseResult flags = options.parse(argc, argv);
  validateFlags(flags);
  std::string dataset = flags["dataset"].as<std::vector<std::string>>()[0];
  fs::path scenePath(dataset);
  if (isStudioScene(scenePath)) {
    Studio<StudioViewController> studio(dataset);
    loop(studio);
  } else if (isPointCloud(scenePath)) {
    Studio<PointCloudViewController> pcStudio(dataset);
    loop(pcStudio);
  } else if (isPointCloudDirectory(scenePath)) {
    auto pc = findPointCloud(scenePath);
    if (pc.has_value()) {
      Studio<PointCloudViewController> pcStudio(pc.value());
      loop(pcStudio);
    } else {
      std::cout << "The path " << scenePath.string() << " does not look like a point cloud (.ply) or a Stray Scene." << std::endl;
      return 1;
    }
  } else {
    std::cout << "The path " << scenePath.string() << " does not look like a point cloud (.ply) or a Stray Scene." << std::endl;
    return 1;
  }

  return 0;
}
