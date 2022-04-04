#include <filesystem>
#include <memory>
#include "geometry/point_cloud.h"

namespace fs = std::filesystem;

namespace model {
class PointCloudDataset {
private:
  fs::path path;
  fs::path currentPointCloud;
  std::shared_ptr<geometry::PointCloud> currentCloud;
  std::vector<fs::path> pointClouds;
  int currentIndex = -1;
public:
  PointCloudDataset(fs::path current) : path(current.parent_path()), currentPointCloud(current) {
    indexPointClouds();

    auto it = find_if(pointClouds.begin(), pointClouds.end(), [&](const fs::path pc) {
      return pc == current;
    });
    currentIndex = it - pointClouds.begin();
    currentCloud = std::make_shared<geometry::PointCloud>(currentPointCloud.string());
  }

  std::shared_ptr<geometry::PointCloud> getCurrentCloud() const { return currentCloud; }

  fs::path currentPath() const { return currentPointCloud; }
  std::shared_ptr<geometry::PointCloud> next() {
    currentIndex = (currentIndex + 1) % int(pointClouds.size());
    currentPointCloud = pointClouds[currentIndex];
    return std::make_shared<geometry::PointCloud>(currentPointCloud.string());
  }

private:
  void indexPointClouds() {
    for (auto& p : fs::directory_iterator(path)) {
      fs::path file = p.path();
      std::string extension = file.extension().string();
      if (extension == ".ply") {
        pointClouds.push_back(file);
      }
    }
  }
};
}
