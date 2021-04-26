#pragma once
#include <bgfx/platform.h>
#include "glfw_app.h"
#include "scene_model.h"
#include "timeline.h"
#include "controllers/studio_view_controller.h"

#if BX_PLATFORM_OSX
const unsigned int CommandModifier = GLFW_MOD_SUPER;
#else
const unsigned int CommandModifier = GLFW_MOD_CONTROL;
#endif

using namespace commands;
class LabelStudio : public GLFWApp {
private:
  const std::filesystem::path datasetFolder;
  Timeline timeline;

public:
  SceneModel sceneModel;
  StudioViewController studioViewController;

  LabelStudio(const std::string& folder);

  void leftButtonDown(double x, double y);
  void leftButtonUp(double x, double y);
  void mouseMoved(double x, double y);
  void scroll(double xoffset, double yoffset);
  void resize(int newWidth, int newHeight);
  bool update() const override;

  void undo();

protected:
  void loadState();
};
