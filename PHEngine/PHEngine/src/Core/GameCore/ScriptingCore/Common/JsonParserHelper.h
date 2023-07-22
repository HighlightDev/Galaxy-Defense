#pragma once

#include <glm/vec3.hpp>
#include <json/json.hpp>
#include <string>

namespace EngineCore
{
    namespace Scripts
    {
        class JsonParserHelper
        {
        public:
            static std::string FromJsonToString(const std::string &jsonKey, const nlohmann::json &jsonObject);
            static float FromJsonToFloat(const std::string &jsonKey, const nlohmann::json &jsonObject);
            static glm::vec3 FromJsonToVec3(const std::string &jsonKey, const nlohmann::json &jsonObject);
            static glm::vec3 FromJsonToVec3Color(const std::string &jsonKey, const nlohmann::json &jsonObject);
            static int32_t FromJsonToInt(const std::string &jsonKey, const nlohmann::json &jsonObject);
        };
    }
}
