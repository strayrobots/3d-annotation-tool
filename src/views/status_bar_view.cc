#include <iostream>
#include <bx/file.h>
#include <bx/math.h>
#include <filesystem>
#include <bgfx/platform.h>
#include "views/status_bar_view.h"

static const char* fontCandidates[] = {
  "../assets/SourceSansPro.ttf"
};

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

StatusBarView::StatusBarView(const SceneModel& model, int viewId) : views::View3D(viewId), model(model) {
  fontManager = new FontManager(512);
  bufferManager = new TextBufferManager(fontManager);

  auto path = std::find_if(std::begin(fontCandidates), std::end(fontCandidates), [](const char* path) {
    return std::filesystem::exists(path);
  });
  if (path == std::end(fontCandidates)) {
    std::cout << "Can't find a single font." << std::endl;
    exit(1);
  }
  TrueTypeHandle fontFile = loadTtf(fontManager, *path);
  fontHandle = fontManager->createFontByPixelSize(fontFile, 0, StatusBarFontSize, FONT_TYPE_DISTANCE);
  fontManager->preloadGlyph(fontHandle, L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789:. \n");
  fontManager->destroyTtf(fontFile);

  textBuffer = bufferManager->createTextBuffer(FONT_TYPE_DISTANCE, BufferType::Transient);
}

StatusBarView::~StatusBarView() {
}

void StatusBarView::render(const Rect& rect) const {
  const bx::Vec3 at  = { 0.0f, 0.0f,  0.0f };
  const bx::Vec3 eye = { 0.0f, 0.0f, -1.0f };
  float view[16];
  bx::mtxLookAt(view, eye, at);
  const float centering = 0.5f;
  const bgfx::Caps* caps = bgfx::getCaps();

  {
    float ortho[16];
    bx::mtxOrtho(ortho,
        centering,
        rect.width  + centering,
        rect.height + centering,
        centering,
        0.0f,
        100.0f,
        0.0f,
        caps->homogeneousDepth);
      bgfx::setViewTransform(viewId, view, ortho);
  }
  const int padding = (StatusBarHeight - StatusBarFontSize) / 2 - 3;
  bgfx::setViewRect(viewId, rect.x + padding, rect.y + padding, rect.width, rect.height);
  bgfx::setState(BGFX_STATE_DEFAULT | BGFX_STATE_MSAA);
  bufferManager->clearTextBuffer(textBuffer);
  bufferManager->appendText(textBuffer, fontHandle, "Tool: ");
  switch (model.activeToolId) {
    case AddKeypointToolId:
      bufferManager->appendText(textBuffer, fontHandle, "Keypoint");
      break;
    case MoveKeypointToolId:
      bufferManager->appendText(textBuffer, fontHandle, "Move");
      break;
    case BBoxToolId:
      bufferManager->appendText(textBuffer, fontHandle, "Bounding Box");
      break;
  }
  bufferManager->submitTextBuffer(textBuffer, viewId);
};
}
