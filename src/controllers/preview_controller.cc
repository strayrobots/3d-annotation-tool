#include <random>
#include <algorithm>
#include <filesystem>
#include <vector>
#include <chrono>
#include "controllers/preview_controller.h"

namespace fs = std::filesystem;

namespace controllers {
PreviewController::PreviewController(const SceneModel& scene, int viewId) : viewId(viewId), model(scene) {
  setRandomImage();
}

void PreviewController::viewWillAppear(int w, int h) {
  bgfx::setViewClear(viewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0xff3030ff, 1.0f);
  bgfx::setViewClear(viewId, BGFX_CLEAR_DEPTH, 1.0f);
}

bool PreviewController::leftButtonUp(const ViewContext3D& viewContext) {
  if (rect.hit(viewContext)) {
    setRandomImage();
    return true;
  }
  return false;
}

bool PreviewController::leftButtonDown(const ViewContext3D& viewContext) {
  return rect.hit(viewContext);
}

void PreviewController::render() const {
  bgfx::setViewRect(viewId, rect.x, rect.y, rect.width, rect.height);
  imageView->render();
}

void PreviewController::setRandomImage() {
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::mt19937 rng(seed);
  auto images = model.imagePaths();
  std::vector<fs::path> sampled;
  std::sample(images.begin(),
      images.end(),
      std::back_inserter(sampled),
      1,
      rng);

  imageView = std::make_unique<views::ImagePane>(sampled[0], viewId);
}

}
