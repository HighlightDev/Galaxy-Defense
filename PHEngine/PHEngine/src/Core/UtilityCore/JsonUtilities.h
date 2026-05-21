#pragma once

#include <glm/gtc/quaternion.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <json/json.hpp>

namespace nlohmann_utilities {
glm::vec3 GetRgbFromJsonMap(const nlohmann::json& root);

glm::vec4 GetRgbaFromJsonMap(const nlohmann::json& root);

glm::vec3 GetXyzFromJsonMap(const nlohmann::json& root);

glm::vec2 GetXyFromJsonMap(const nlohmann::json& root);

int32_t GetIntFromJson(const nlohmann::json& root, const std::string& key);

std::string GetStringFromJson(const nlohmann::json& root, const std::string& key);

float GetFloatFromJson(const nlohmann::json& root, const std::string& key);

bool GetBoolFromJson(const nlohmann::json& root, const std::string& key);

glm::quat GetQuatFromJson(const nlohmann::json& json);
} // namespace nlohmann_utilities
