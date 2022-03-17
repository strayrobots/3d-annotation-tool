#pragma once

#include "3rdparty/json.hpp"
#include <eigen3/Eigen/Dense>
namespace utils::serialize {

Eigen::Vector3f toVector3(const nlohmann::json& json);

Eigen::Vector2f toVector2(const nlohmann::json& json);

Eigen::Quaternionf toQuaternion(const nlohmann::json& json);

} // namespace utils::serialize
