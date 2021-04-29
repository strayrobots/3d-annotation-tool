#pragma once
#include "scene_model.h"
#include "commands/command.h"
#include "view_context_3d.h"
#include "timeline.h"

using namespace commands;
namespace tools {

class Tool {
protected:
  SceneModel& sceneModel;
  Timeline& timeline;

public:
  Tool(SceneModel& sceneModel, Timeline& timeline);
  virtual ~Tool(){};
  virtual bool leftButtonDown(const ViewContext3D& context) { return false; };
  virtual bool leftButtonUp(const ViewContext3D& context) { return false; };
  virtual bool mouseMoved(const ViewContext3D& context) { return false; }
};
} // namespace tools
