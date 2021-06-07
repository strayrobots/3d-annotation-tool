#pragma once
#include <bgfx/platform.h>
#include "glfw_app.h"
#include "scene_model.h"
#include "timeline.h"
#include "controllers/studio_view_controller.h"

using namespace commands;
class LabelStudio : public GLFWApp {
private:
  int viewId;

public:
  SceneModel sceneModel;
  StudioViewController studioViewController;
  InputModifier inputModifier = ModNone;

  LabelStudio(const std::string& folder);

  void leftButtonDown(double x, double y);
  void setInputModifier(int mods);
  void leftButtonUp(double x, double y);
  void mouseMoved(double x, double y);
  void scroll(double xoffset, double yoffset);
  void resize(int newWidth, int newHeight) override;
  bool update() const override;

  void undo();

protected:
  Timeline timeline;
  const std::filesystem::path datasetFolder;

  void loadState();
};
