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
  Timeline(SceneModel& model) : sceneModel(model), commandStack() {
  }
  Timeline(Timeline const& other) = delete;

  void load(fs::path annotationPath) {
    commandStack.clear();
    if (!std::filesystem::exists(annotationPath))
      return;
    std::ifstream file(annotationPath.string());
    nlohmann::json json;
    file >> json;
    for (auto& keypoint : json["keypoints"]) {
      auto position = keypoint["position"];
      auto k = Vector3f(position[0].get<float>(), position[1].get<float>(), position[2].get<float>());
      Keypoint kp(-1, keypoint["class_id"].get<int>(), k);
      std::unique_ptr<Command> command = std::make_unique<commands::AddKeypointCommand>(kp);
      pushCommand(std::move(command));
    }
    for (auto& bbox : json["bounding_boxes"]) {
      auto p = bbox["position"];
      auto orn = bbox["orientation"];
      auto d = bbox["dimensions"];
      auto classId = bbox["class_id"];
      BBox box = {
          .classId = classId,
          .position = utils::serialize::toVector3(p),
          .orientation = Quaternionf(orn["w"].get<float>(), orn["x"].get<float>(), orn["y"].get<float>(), orn["z"].get<float>()),
          .dimensions = utils::serialize::toVector3(d),
      };
      std::unique_ptr<Command> command = std::make_unique<commands::AddBBoxCommand>(box);
      pushCommand(std::move(command));
    }
    for (auto& rectangle : json["rectangles"]) {
      Rectangle rect(0, rectangle["class_id"],
                     utils::serialize::toVector3(rectangle["center"]),
                     utils::serialize::toQuaternion(rectangle["orientation"]),
                     utils::serialize::toVector2(rectangle["size"]));
      std::unique_ptr<Command> command = std::make_unique<commands::AddRectangleCommand>(rect);
      pushCommand(std::move(command));
    }
  }

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
