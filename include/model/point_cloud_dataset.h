#include <filesystem>

namespace fs = std::filesystem;

namespace model {
class PointCloudDataset {
private:
  fs::path path;
  fs::path currentPointCloud;
  std::vector<fs::path> pointClouds;
  int currentIndex = -1;
public:
  PointCloudDataset(fs::path current) : path(current.parent_path()), currentPointCloud(current) {
    indexPointClouds();

    auto it = find_if(pointClouds.begin(), pointClouds.end(), [&](const fs::path pc) {
      return pc == current;
    });
    currentIndex = it - pointClouds.begin();
  }

  fs::path next() {
    currentIndex = (currentIndex + 1) % int(pointClouds.size());
    return pointClouds[currentIndex];
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
