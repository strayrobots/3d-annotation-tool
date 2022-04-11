#pragma once
#include <list>
#include <memory>
#include "commands/command.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include "utils/serialize.h"
#include "commands/keypoints.h"
#include "commands/bounding_box.h"
#include "commands/rectangle.h"

using CommandPtr = std::unique_ptr<commands::Command>;
using CommandStack = std::list<std::unique_ptr<commands::Command>>;

using namespace commands;

namespace fs = std::filesystem;
class Timeline {
private:
  SceneModel& sceneModel;
  CommandStack commandStack;

public:
  Timeline(SceneModel& model);
  Timeline(Timeline const& other) = delete;
  void load(fs::path annotationPath);
  void pushCommand(CommandPtr command);
  void undoCommand();
  int size() const;
};
