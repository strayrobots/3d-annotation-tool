#include <memory>
#include <thread>
#include <chrono>
#include <iostream>
#include <filesystem>
#include "3rdparty/cxxopts.h"
#include "glfw_app.h"
#include "views/image_pane.h"

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
    if (!fs::exists(dataset)) {
      std::cout << "Dataset folder does not exist." << std::endl;
      valid = false;
    }
  }
  if (!valid) {
    exit(1);
  }
}


class PreviewApp  : public GLFWApp {
private:
  const fs::path datasetPath;
  std::vector<fs::path> colorImages;
  std::unique_ptr<views::ImagePane> imageView;
  int currentFrame = 0;
public:
  PreviewApp(const std::string& folder) : GLFWApp("Stray Preview"), datasetPath(folder) {
    listImages();
    imageView = std::make_unique<views::ImagePane>(colorImages[0]);
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR|BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);
  }

  bool advance() {
    currentFrame++;
    if (currentFrame < colorImages.size()) {
      imageView->setImage(colorImages[currentFrame]);
    }
  }

  bool update() const override {
    if (currentFrame >= colorImages.size()) return false;
    bgfx::setViewRect(0, 0, 0, width, height);
    imageView->render();
    bgfx::frame();
    glfwWaitEventsTimeout(0.03);
    return !glfwWindowShouldClose(window);
  }

private:
  void listImages() {
    auto colorDir = datasetPath / "color";
    for (auto& p : fs::directory_iterator(colorDir)) {
      colorImages.push_back(p.path());
    }
    std::sort(colorImages.begin(), colorImages.end());
  }
};

int main(int argc, char* argv[]) {
  cxxopts::Options options("LabelStudio", "Annotate the world in 3D.");
  options.add_options()("dataset", "That path to folder of the dataset to annotate.",
      cxxopts::value<std::vector<std::string>>());
  options.parse_positional({"dataset"});
  cxxopts::ParseResult flags = options.parse(argc, argv);
  validateFlags(flags);
  std::string dataset = flags["dataset"].as<std::vector<std::string>>()[0];

  PreviewApp window(dataset);

  while (window.update()) {
    window.advance();
  }

  return 0;
}

