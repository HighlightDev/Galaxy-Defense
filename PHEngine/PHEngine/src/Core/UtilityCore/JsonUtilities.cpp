#include "JsonUtilities.h"

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
            assert(false);
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
            assert(false);
        }
    }
    return result;
}

int32_t GetIntFromJson(const nlohmann::json& root)
{
    return root.get<int32_t>();
}

std::string GetStringFromJson(const nlohmann::json& root)
{
    return root.get<std::string>();
}

float GetFloatFromJson(const nlohmann::json& root)
{
    return root.get<float>();
}
} // namespace nlohmann_utilities
