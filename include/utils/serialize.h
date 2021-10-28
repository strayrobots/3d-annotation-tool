#include "3rdparty/json.hpp"

namespace utils::serialize {

Vector3f toVector3(const nlohmann::json& json) {
  return Vector3f(json[0].get<float>(), json[1].get<float>(), json[2].get<float>());
}

Vector2f toVector2(const nlohmann::json& json) {
  return Vector2f(json[0].get<float>(), json[1].get<float>());
}

Quaternionf toQuaternion(const nlohmann::json& json) {
  return Quaternionf(json["w"].get<float>(), json["x"].get<float>(), json["y"].get<float>(), json["z"].get<float>());
}

}
