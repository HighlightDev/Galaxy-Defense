#include "JsonParserHelper.h"
#include "Core/CommonCore/Assertion.h"

namespace EngineCore
{
    namespace Scripts
    {
        std::string JsonParserHelper::FromJsonToString(const std::string &jsonKey, const nlohmann::json &jsonObject)
        {
            assert(jsonObject.contains(jsonKey));
            return jsonObject[jsonKey].get<std::string>();
        }

        float JsonParserHelper::FromJsonToFloat(const std::string &jsonKey, const nlohmann::json &jsonObject)
        {
            assert(jsonObject.contains(jsonKey));
            return jsonObject[jsonKey].get<float>();
        }

        glm::vec3 JsonParserHelper::FromJsonToVec3(const std::string &jsonKey, const nlohmann::json &jsonObject)
        {
            assert(jsonObject.contains(jsonKey));
            const auto vecProps = jsonObject[jsonKey];
            glm::vec3 result;
            for (auto it = vecProps.cbegin(); it != vecProps.cend(); ++it)
            {
                const auto key = it.key();
                if ("x" == key)
                {
                    result.x = it->get<float>();
                }
                else if ("y" == key)
                {
                    result.y = it->get<float>();
                }
                else if ("z" == key)
                {
                    result.z = it->get<float>();
                }
                else
                {
                    assert(false);
                }
            }
            return result;
        }

        glm::vec3 JsonParserHelper::FromJsonToVec3Color(const std::string &jsonKey, const nlohmann::json &jsonObject)
        {
            assert(jsonObject.contains(jsonKey));
            const auto vecProps = jsonObject[jsonKey];
            glm::vec3 color;
            for (auto it = vecProps.cbegin(); it != vecProps.cend(); ++it)
            {
                const auto key = it.key();
                if ("r" == key)
                {
                    color.r = it->get<float>();
                }
                else if ("g" == key)
                {
                    color.g = it->get<float>();
                }
                else if ("b" == key)
                {
                    color.b = it->get<float>();
                }
                else
                {
                    assert(false);
                }
            }
            return color;
        }

        int32_t JsonParserHelper::FromJsonToInt(const std::string &jsonKey, const nlohmann::json &jsonObject)
        {
            assert(jsonObject.contains(jsonKey));
            return jsonObject[jsonKey].get<int32_t>();
        }
    }
}
