#include "SceneComponentLuaProxy.h"

#include "Core/UtilityCore/JsonUtilities.h"

#include <json/json.hpp>

namespace EngineCore::Scripts {
SceneComponentLuaProxy::SceneComponentLuaProxy(const std::shared_ptr<SceneComponent>& baseComponent)
    : BaseComponentLuaProxy(baseComponent)
{
}

SceneComponentLuaProxy::~SceneComponentLuaProxy()
{
}

bool SceneComponentLuaProxy::InvokeFunction(const std::string& functionName, const std::string& jsonParameters)
{
    const bool baseInvokeResult = BaseComponentLuaProxy::InvokeFunction(functionName, jsonParameters);
    bool result = false;

    if (!baseInvokeResult) {
        if (const auto ownerComponentSp = mOwnerComponentWp.lock()) {
            const auto sceneComponentSp = std::dynamic_pointer_cast<SceneComponent>(ownerComponentSp);
            if (sceneComponentSp) {
                if ("SetTranslation" == functionName) {
                    const auto json = nlohmann::json::parse(jsonParameters);
                    if (json.contains("translation")) {
                        const glm::vec3 translation = nlohmann_utilities::GetXyzFromJsonMap(json["translation"]);
                        sceneComponentSp->SetTranslation(translation);
                        result = true;
                    } else {
                        result = false;
                    }
                } else if ("SetRotator" == functionName) {
                    const auto json = nlohmann::json::parse(jsonParameters);
                    if (json.contains("rotator")) {
                        const glm::quat rotator = nlohmann_utilities::GetQuatFromJson(json["rotator"]);
                        sceneComponentSp->SetRotator(rotator);
                        result = true;
                    } else {
                        result = false;
                    }
                } else if ("SetScale" == functionName) {
                    const auto json = nlohmann::json::parse(jsonParameters);
                    if (json.contains("scale")) {
                        const glm::vec3 scale = nlohmann_utilities::GetXyzFromJsonMap(json["scale"]);
                        sceneComponentSp->SetScale(scale);
                        result = true;
                    } else {
                        result = false;
                    }
                } else if ("SetAdditionalRotation" == functionName) {
                    const auto json = nlohmann::json::parse(jsonParameters);
                    if (json.contains("rotationDegrees")) {
                        const glm::vec3 rotationDegrees = nlohmann_utilities::GetXyzFromJsonMap(json["rotationDegrees"]);
                        sceneComponentSp->SetAdditionalRotation(rotationDegrees);
                        result = true;
                    } else {
                        result = false;
                    }
                } else if ("AddTranslation" == functionName) {
                    const auto json = nlohmann::json::parse(jsonParameters);
                    if (json.contains("offsetTranslation")) {
                        const glm::vec3 offsetTranslation = nlohmann_utilities::GetXyzFromJsonMap(json["offsetTranslation"]);
                        sceneComponentSp->AddTranslation(offsetTranslation);
                        result = true;
                    } else {
                        result = false;
                    }
                }
            }
        }
    }
    return result || baseInvokeResult;
}
} // namespace EngineCore::Scripts