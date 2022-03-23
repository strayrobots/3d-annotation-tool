#pragma once

#include "3rdparty/json.hpp"
#include <eigen3/Eigen/Dense>
#include "scene_model.h"
namespace utils::serialize {

Eigen::Vector3f toVector3(const nlohmann::json& json);

Eigen::Vector2f toVector2(const nlohmann::json& json);

Eigen::Quaternionf toQuaternion(const nlohmann::json& json);

nlohmann::json serialize(const Eigen::Vector3f& v);
nlohmann::json serialize(const Eigen::Vector2f& v);
nlohmann::json serialize(const Keypoint& keypoint);
nlohmann::json serialize(const BBox& bbox);
nlohmann::json serialize(const Rectangle& rectangle);

} // namespace utils::serialize
