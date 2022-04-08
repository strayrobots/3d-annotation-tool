#include <iostream>
#include <fstream>
#include <filesystem>
#include "timeline.h"
#include "utils/serialize.h"
#include "commands/keypoints.h"
#include "commands/bounding_box.h"
#include "commands/rectangle.h"

using CommandPtr = std::unique_ptr<commands::Command>;
using CommandStack = std::list<std::unique_ptr<commands::Command>>;

using namespace commands;
Timeline::Timeline(SceneModel& model) : sceneModel(model), commandStack() {
}

void Timeline::load(fs::path annotationPath) {
  commandStack.clear();
  if (!std::filesystem::exists(annotationPath))
    return;
  std::ifstream file(annotationPath.string());
  nlohmann::json json;
  file >> json;
  for (auto& keypoint : json["keypoints"]) {
    auto position = keypoint["position"];
    auto k = Vector3f(position[0].get<float>(), position[1].get<float>(), position[2].get<float>());
    int classId = keypoint.value("class_id", 0);
    Keypoint kp(-1, classId, k);
    std::unique_ptr<Command> command = std::make_unique<commands::AddKeypointCommand>(kp);
    pushCommand(std::move(command));
  }
  for (auto& bbox : json["bounding_boxes"]) {
    auto p = bbox["position"];
    auto orn = bbox["orientation"];
    auto d = bbox["dimensions"];
    int classId = bbox.value("classId", 0);
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
    int classId = rectangle.value("class_id", 0);
    Rectangle rect(0, classId,
                   utils::serialize::toVector3(rectangle["center"]),
                   utils::serialize::toQuaternion(rectangle["orientation"]),
                   utils::serialize::toVector2(rectangle["size"]));
    std::unique_ptr<Command> command = std::make_unique<commands::AddRectangleCommand>(rect);
    pushCommand(std::move(command));
  }
}

void Timeline::pushCommand(CommandPtr command) {
  command->execute(sceneModel);
  commandStack.push_back(std::move(command));
}

void Timeline::undoCommand() {
  if (commandStack.empty()) return;
  commandStack.back()->undo(sceneModel);
  commandStack.pop_back();
}

int Timeline::size() const {
  return commandStack.size();
}
