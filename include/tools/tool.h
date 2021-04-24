#pragma once
#include "scene_model.h"
#include "commands/command.h"
#include "view_context_3d.h"

using namespace commands;
namespace tools {

class Tool {
protected:
  SceneModel& sceneModel;
  CommandStack& commandStack;
public:
  Tool(SceneModel& sceneModel, CommandStack& commandStack);
  virtual ~Tool() {};
  virtual void activate() {};
  virtual void deactivate() {};
  virtual bool leftButtonDown(const ViewContext3D& context) { return false; };
  virtual bool leftButtonUp(const ViewContext3D& context) { return false; };
  virtual bool mouseMoved(const ViewContext3D& context) { return false; }
};
}
