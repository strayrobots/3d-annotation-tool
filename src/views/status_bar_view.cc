#include <iostream>
#include <bx/file.h>
#include <bx/math.h>
#include <filesystem>
#include <bgfx/platform.h>
#include <sstream>
#include "views/status_bar_view.h"
#include "asset_utils.h"

const uint32_t TextColor = 0xffffffff;
const uint32_t StatusBarColor = 0x222222ff;
const std::string fontCandidates[] = {
    "SourceSansPro.ttf"};

static bx::DefaultAllocator allocator;

namespace views {

void* load(const char* file, uint32_t* readSize) {
  bx::FileReader reader;
  if (bx::open(&reader, file)) {
    uint32_t size = (uint32_t)bx::getSize(&reader);
    void* data = BX_ALLOC(&allocator, size);
    bx::read(&reader, data, size);
    *readSize = size;
    bx::close(&reader);
    return data;
  } else {
    std::cout << "Failed to open file: " << file << std::endl;
  }
  return nullptr;
}

TrueTypeHandle loadTtf(FontManager* fontManager, const char* filePath) {
  uint32_t size;
  void* data = load(filePath, &size);
  if (NULL != data) {
    TrueTypeHandle handle = fontManager->createTtf((uint8_t*)data, size);
    BX_FREE(&allocator, data);
    return handle;
  }

  TrueTypeHandle invalid = BGFX_INVALID_HANDLE;
  return invalid;
}

StatusBarView::StatusBarView(const SceneModel& model, int viewId) : views::View(viewId), model(model) {
  fontManager = new FontManager(512);
  bufferManager = new TextBufferManager(fontManager);

  std::string assetDir = asset_utils::findAssetDirectory();
  auto path = std::find_if(std::begin(fontCandidates), std::end(fontCandidates), [&](const std::string& path) {
    return std::filesystem::exists(assetDir + path);
  });
  if (path == std::end(fontCandidates)) {
    std::cout << "Can't find a single font." << std::endl;
    exit(1);
  }
  std::string fontPath = assetDir + *path;
  TrueTypeHandle fontFile = loadTtf(fontManager, fontPath.c_str());
  fontHandle = fontManager->createFontByPixelSize(fontFile, 0, StatusBarFontSize, FONT_TYPE_DISTANCE);
  fontManager->preloadGlyph(fontHandle, L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789:. \n");
  fontManager->destroyTtf(fontFile);

  toolText = bufferManager->createTextBuffer(FONT_TYPE_DISTANCE, BufferType::Transient);
  bufferManager->setTextColor(toolText, TextColor);
  instanceIdText = bufferManager->createTextBuffer(FONT_TYPE_DISTANCE, BufferType::Transient);
  bufferManager->setTextColor(instanceIdText, TextColor);
  bgfx::setViewClear(viewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, StatusBarColor, 0.0f, 0);

  bufferManager->appendText(instanceIdText, fontHandle, "Instance id: 1");
  auto rect = bufferManager->getRectangle(instanceIdText);
  instanceTextWidth = rect.width;
}

StatusBarView::~StatusBarView() {
}

void StatusBarView::render() const {
  const bx::Vec3 at = {0.0f, 0.0f, 0.0f};
  const bx::Vec3 eye = {0.0f, 0.0f, -1.0f};
  float view[16];
  bx::mtxLookAt(view, eye, at);
  const float centering = 0.5f;
  const bgfx::Caps* caps = bgfx::getCaps();
  const float padding = 10.0f;

  {
    float ortho[16];
    bx::mtxOrtho(ortho,
                 centering,
                 rect.width + centering,
                 rect.height + centering,
                 centering,
                 0.0f,
                 100.0f,
                 0.0f,
                 caps->homogeneousDepth);
    bgfx::setViewTransform(viewId, view, ortho);
  }
  bgfx::setViewRect(viewId, rect.x, rect.y, rect.width, rect.height);
  bufferManager->clearTextBuffer(toolText);
  bufferManager->setPenPosition(toolText, padding, 0.0f);
  bufferManager->appendText(toolText, fontHandle, "Tool: ");
  switch (model.activeToolId) {
  case AddKeypointToolId:
    bufferManager->appendText(toolText, fontHandle, "Keypoint");
    break;
  case MoveKeypointToolId:
    bufferManager->appendText(toolText, fontHandle, "Move");
    break;
  case BBoxToolId:
    bufferManager->appendText(toolText, fontHandle, "Bounding Box");
    break;
  case AddRectangleToolId:
    bufferManager->appendText(toolText, fontHandle, "Rectangle");
    break;
  }
  bufferManager->submitTextBuffer(toolText, viewId);
  bufferManager->clearTextBuffer(instanceIdText);
  bufferManager->setPenPosition(instanceIdText, rect.width - instanceTextWidth - padding, 0.0f);
  std::stringstream stream;
  stream << "Instance id: " << model.currentInstanceId;
  bufferManager->appendText(instanceIdText, fontHandle, stream.str().c_str());
  bufferManager->submitTextBuffer(instanceIdText, viewId);
};
} // namespace views
