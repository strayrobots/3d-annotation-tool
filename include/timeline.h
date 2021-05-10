#pragma once
#include <list>
#include <memory>
#include "commands/command.h"

using CommandPtr = std::unique_ptr<commands::Command>;
using CommandStack = std::list<std::unique_ptr<commands::Command>>;

class Timeline {
private:
  SceneModel& sceneModel;
  CommandStack commandStack;

public:
  Timeline(SceneModel& model) : sceneModel(model), commandStack() {
  }
  Timeline(Timeline const& other) = delete;

  void pushCommand(CommandPtr command) {
    command->execute(sceneModel);
    commandStack.push_back(std::move(command));
  }

  void undoCommand() {
    if (commandStack.empty()) return;
    commandStack.back()->undo(sceneModel);
    commandStack.pop_back();
  }

  int size() const {
    return commandStack.size();
  }
};
