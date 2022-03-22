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

nlohmann::json serialize(const Eigen::Vector3f& v) {
  auto out = nlohmann::json::array();
  out[0] = v[0];
  out[1] = v[1];
  out[2] = v[2];
  return out;
}

nlohmann::json serialize(const Eigen::Vector2f& v) {
  auto out = nlohmann::json::array();
  out[0] = v[0];
  out[1] = v[1];
  return out;
}

nlohmann::json serialize(const Keypoint& keypoint) {
  auto out = nlohmann::json::object();
  out["instance_id"] = keypoint.instanceId;
  out["position"] = serialize(keypoint.position);
  return out;
}

nlohmann::json serialize(const BBox& bbox) {
  auto obj = nlohmann::json::object();
  obj["position"] = serialize(bbox.position);
  obj["orientation"] = {
      {"w", bbox.orientation.w()},
      {"x", bbox.orientation.x()},
      {"y", bbox.orientation.y()},
      {"z", bbox.orientation.z()}};
  obj["dimensions"] = serialize(bbox.dimensions);
  obj["instance_id"] = bbox.instanceId;
  return obj;
}

nlohmann::json serialize(const Rectangle& rectangle) {
  auto obj = nlohmann::json::object();
  obj["center"] = serialize(rectangle.center);
  obj["orientation"] = {
      {"w", rectangle.orientation.w()},
      {"x", rectangle.orientation.x()},
      {"y", rectangle.orientation.y()},
      {"z", rectangle.orientation.z()}};
  obj["size"] = serialize(rectangle.size);
  obj["class_id"] = rectangle.classId;
  return obj;
}

} // namespace utils::serialize
