#include <algorithm>
#include <chrono>
#include <filesystem>
#include <random>
#include <vector>
#include <string>
#include "controllers/preview_controller.h"
#include "id.h"

namespace fs = std::filesystem;

namespace controllers {
PreviewController::PreviewController(const SceneModel& scene, int viewId) : viewId(viewId),
    model(scene), viewContext(model.sceneCamera()) {
  annotationView = std::make_unique<views::AnnotationView>(model, IdFactory::getInstance().getId());
  setRandomImage();
}

void PreviewController::viewWillAppear(const views::Rect& rect) {
  Controller3D::viewWillAppear(rect);
  viewContext.width = rect.width;
  viewContext.height = rect.height;
  bgfx::setViewClear(viewId, BGFX_CLEAR_DEPTH, 1.0f);
  bgfx::setViewClear(annotationView->viewId, BGFX_CLEAR_DEPTH, 1.0f);
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

bool PreviewController::keypress(char keypress, const InputModifier mod) {
  if (keypress == 'R' && mod == ModNone) {
    setRandomImage();
    return true;
  }
  return false;
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

  fs::path imagePath = sampled[0];
  int imageIndex = std::stoi(imagePath.stem());
  Matrix4f T_CW = model.cameraTrajectory()[imageIndex];
  Vector3f p_C = T_CW.block<3, 1>(0, 3);
  Quaternionf R_C(T_CW.block<3, 3>(0, 0));
  auto R_WC = AngleAxisf(M_PI, Vector3f::UnitX());
  viewContext.camera.setOrientation((R_C * R_WC).normalized());
  viewContext.camera.setPosition(p_C);
  imageView = std::make_unique<views::ImagePane>(sampled[0], viewId);
}

void PreviewController::render() const {
  bgfx::setViewRect(viewId, rect.x, rect.y, rect.width, rect.height);
  imageView->render();
  bgfx::setViewRect(annotationView->viewId, rect.x, rect.y, rect.width, rect.height);
  bgfx::setDebug(BGFX_DEBUG_TEXT);
  annotationView->render(viewContext);
}

}
