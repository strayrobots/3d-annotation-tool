#include <filesystem>
#include <vector>
#include <memory>
#include <future>
#include <thread>
#include "geometry/point_cloud.h"

namespace fs = std::filesystem;

namespace model {
using PointCloudPtr = std::shared_ptr<geometry::PointCloud>;

class PointCloudDataset {
private:
  fs::path path;
  fs::path currentPointCloud;
  std::shared_future<PointCloudPtr> currentCloud, nextCloud;
  std::vector<fs::path> pointClouds;
  int currentIndex = -1;

public:
  PointCloudDataset(fs::path current);
  const std::shared_future<PointCloudPtr>& getCurrentCloud() const;
  fs::path currentPath() const;
  fs::path nextPath() const;
  std::shared_future<PointCloudPtr> next();

private:
  void indexPointClouds();
  std::shared_future<PointCloudPtr> fetchPointCloud(fs::path pcPath);
};
} // namespace model
