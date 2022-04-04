#include "model/point_cloud_dataset.h"

namespace model {

PointCloudDataset::PointCloudDataset(fs::path current) : path(current.parent_path()), currentPointCloud(current) {
  indexPointClouds();

  auto it = find_if(pointClouds.begin(), pointClouds.end(), [&](const fs::path pc) {
    return pc == current;
  });
  currentIndex = it - pointClouds.begin();
  currentCloud = fetchPointCloud(currentPointCloud);
  nextCloud = fetchPointCloud(nextPath());
}

const std::shared_future<PointCloudPtr>& PointCloudDataset::getCurrentCloud() const {
  return currentCloud;
}

fs::path PointCloudDataset::currentPath() const { return currentPointCloud; }
fs::path PointCloudDataset::nextPath() const {
  int nextIndex = (currentIndex + 1) % int(pointClouds.size());
  return pointClouds[nextIndex];
}

std::shared_future<PointCloudPtr> PointCloudDataset::next() {
  currentIndex = (currentIndex + 1) % int(pointClouds.size());
  currentPointCloud = pointClouds[currentIndex];
  currentCloud = nextCloud;
  nextCloud = fetchPointCloud(nextPath());
  return currentCloud;
}

void PointCloudDataset::indexPointClouds() {
  for (auto& p : fs::directory_iterator(path)) {
    fs::path file = p.path();
    std::string extension = file.extension().string();
    if (extension == ".ply") {
      pointClouds.push_back(file);
    }
  }
}

std::shared_future<PointCloudPtr> PointCloudDataset::fetchPointCloud(fs::path pcPath) {
  auto path = nextPath();
  std::promise<PointCloudPtr> promise;
  std::shared_future<PointCloudPtr> theFuture = promise.get_future();
  auto getFunction = [&, theFuture]() -> PointCloudPtr {
    PointCloudPtr ptr = std::make_shared<geometry::PointCloud>(pcPath.string());
    promise.set_value(ptr);
    theFuture.wait();
    return ptr;
  };
  auto result = std::async(std::launch::async, getFunction);
  return theFuture;
}

}
