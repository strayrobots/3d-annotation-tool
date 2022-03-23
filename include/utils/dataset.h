#pragma once
#include <vector>
#include <filesystem>
#include <eigen3/Eigen/Dense>
#include "scene_model.h"
#include "timeline.h"

namespace fs = std::filesystem;

namespace utils::dataset {

fs::path getAnnotationPathForPointCloudPath(const fs::path& path);
std::vector<fs::path> getDatasetImagePaths(fs::path path);
std::vector<Eigen::Matrix4f> getDatasetCameraTrajectory(fs::path path);
DatasetMetadata getDatasetMetadata(fs::path path);
std::vector<fs::path> getDatasetPointCloudPaths(fs::path path);

}; // namespace utils::dataset