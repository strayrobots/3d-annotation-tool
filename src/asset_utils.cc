#include "asset_utils.h"
#include <filesystem>
#include <iostream>
#include <boost/dll.hpp>

namespace fs = std::filesystem;

namespace asset_utils {
fs::path findAssetDirectory() {
  fs::path currentPath(boost::dll::program_location().string());
  auto sharePath = currentPath.parent_path().parent_path() / "share" / "stray" / "assets/";
  if (std::filesystem::exists("../assets") && std::filesystem::exists("../assets/x_axis.ply")) {
    // For development, load assets from parent directory.
    return "../assets/";
  } else if (std::filesystem::exists(sharePath.string())) {
    return sharePath;
  } else {
    std::cout << "Can't find asset directory." << std::endl;
    exit(EXIT_FAILURE);
  }
}
}
