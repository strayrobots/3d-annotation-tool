#include "asset_utils.h"
#include <filesystem>
#include <iostream>

namespace asset_utils {
std::string findAssetDirectory() {
  if (std::filesystem::exists("../assets") && std::filesystem::exists("../assets/x_axis.ply")) {
    // For development, load assets from parent directory.
    return "../assets/";
  } else if (std::filesystem::exists("/usr/local/share/stray/assets")) {
    return "/usr/local/share/stray/assets/";
  } else {
    std::cout << "Can't find asset directory." << std::endl;
    exit(EXIT_FAILURE);
  }
}
}
