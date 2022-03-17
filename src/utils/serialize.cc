#include "3rdparty/json.hpp"
#include "utils/serialize.h"
#include <eigen3/Eigen/Dense>

namespace utils::serialize {

Eigen::Vector3f toVector3(const nlohmann::json& json) {
  return Eigen::Vector3f(json[0].get<float>(), json[1].get<float>(), json[2].get<float>());
}

Eigen::Vector2f toVector2(const nlohmann::json& json) {
  return Eigen::Vector2f(json[0].get<float>(), json[1].get<float>());
}

Eigen::Quaternionf toQuaternion(const nlohmann::json& json) {
  return Eigen::Quaternionf(json["w"].get<float>(), json["x"].get<float>(), json["y"].get<float>(), json["z"].get<float>());
}

} // namespace utils::serialize
