#include <thread>
#include <chrono>
#include <iostream>
#include <filesystem>
#include "3rdparty/cxxopts.h"
#include "studio.h"
#include "controllers/point_cloud_view_controller.h"

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

int main(int argc, char* argv[]) {
  cxxopts::Options options("Stray Point Cloud Studio", "Annotate the world.");
  options.add_options()("dataset", "That path to the directory containing point clouds to annotate.",
                        cxxopts::value<std::vector<std::string>>());
  options.parse_positional({"dataset"});
  cxxopts::ParseResult flags = options.parse(argc, argv);
  validateFlags(flags);
  std::string dataset = flags["dataset"].as<std::vector<std::string>>()[0];

  Studio<PointCloudViewController> window(dataset);

  while (window.update()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
  }

  return 0;
}