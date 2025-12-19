#include "JsonUtilities.h"

#include "Core/CommonCore/Assertion.h"

namespace nlohmann_utilities {
glm::vec3 GetRgbFromJsonMap(const nlohmann::json& root)
{
    glm::vec3 color;
    for (auto it = root.cbegin(); it != root.cend(); ++it) {
        const auto key = it.key();
        if ("r" == key) {
            color.r = it->get<float>();
        } else if ("g" == key) {
            color.g = it->get<float>();
        } else if ("b" == key) {
            color.b = it->get<float>();
        } else {
            ext_assert(false, "Missing r or g or b key in json object");
        }
    }

    return color;
}

glm::vec4 GetRgbaFromJsonMap(const nlohmann::json& root)
{
    glm::vec4 color(1.0f);
    for (auto it = root.cbegin(); it != root.cend(); ++it) {
        const auto key = it.key();
        if ("r" == key) {
            color.r = it->get<float>();
        } else if ("g" == key) {
            color.g = it->get<float>();
        } else if ("b" == key) {
            color.b = it->get<float>();
        } else if ("a" == key) {
            color.a = it->get<float>();
        } else {
            ext_assert(false, "Invalid color key in json object: " + key);
        }
    }

    return color;
}

glm::vec3 GetXyzFromJsonMap(const nlohmann::json& root)
{
    glm::vec3 result;
    for (auto it = root.cbegin(); it != root.cend(); ++it) {
        const auto key = it.key();
        if ("x" == key) {
            result.x = it->get<float>();
        } else if ("y" == key) {
            result.y = it->get<float>();
        } else if ("z" == key) {
            result.z = it->get<float>();
        } else {
            ext_assert(false, "Missing x or y or z key in json object");
        }
    }
    return result;
}

int32_t GetIntFromJson(const nlohmann::json& root, const std::string& key)
{
    ext_assert(root.contains(key), "Missing key in json object: " + key);
    return root.at(key).get<int32_t>();
}

std::string GetStringFromJson(const nlohmann::json& root, const std::string& key)
{
    ext_assert(root.contains(key), "Missing key in json object: " + key);
    return root.at(key).get<std::string>();
}

float GetFloatFromJson(const nlohmann::json& root, const std::string& key)
{
    ext_assert(root.contains(key), "Missing key in json object: " + key);
    return root.at(key).get<float>();
}

bool GetBoolFromJson(const nlohmann::json& root, const std::string& key)
{
    ext_assert(root.contains(key), "Missing key in json object: " + key);
    return root.at(key).get<bool>();
}
} // namespace nlohmann_utilities
