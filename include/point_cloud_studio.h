#pragma once
#include <bgfx/platform.h>
#include "glfw_app.h"
#include "scene_model.h"
#include "timeline.h"
#include "controllers/point_cloud_studio_view_controller.h"

using namespace commands;
class PointCloudStudio : public GLFWApp {
public:
  PointCloudStudioSceneModel sceneModel;
  PointCloudStudioViewController studioViewController;
  InputModifier inputModifier = ModNone;

  PointCloudStudio(const std::string& folder);

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
