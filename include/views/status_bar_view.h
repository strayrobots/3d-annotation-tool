#pragma once
#include "scene_model.h"
#include "common/font/font_manager.h"
#include "common/font/text_buffer_manager.h"
#include "views/view.h"

namespace views {

const int StatusBarFontSize = 16;
const int StatusBarHeight = 24;

class StatusBarView : public views::View {
private:
  const SceneModel& model;
  FontManager* fontManager;
  TextBufferManager* bufferManager;
  FontHandle fontHandle;
  TextBufferHandle toolText;
  TextBufferHandle instanceIdText;
  int instanceTextWidth;

public:
  StatusBarView(const SceneModel& model, int viewId);
  ~StatusBarView();
  void render() const;
};
} // namespace views
