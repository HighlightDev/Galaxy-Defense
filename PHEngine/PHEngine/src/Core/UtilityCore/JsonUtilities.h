#pragma once

#include <glm/vec3.hpp>
#include <json/json.hpp>

namespace nlohmann_utilities {
glm::vec3 GetRgbFromJsonMap(const nlohmann::json& root);

glm::vec3 GetXyzFromJsonMap(const nlohmann::json& root);

int32_t GetIntFromJson(const nlohmann::json& root);

std::string GetStringFromJson(const nlohmann::json& root);

float GetFloatFromJson(const nlohmann::json& json);
} // namespace nlohmann_utilities
