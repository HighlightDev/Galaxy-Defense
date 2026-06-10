#include "ElectricBeamComponentLuaProxy.h"

#include "Core/UtilityCore/JsonUtilities.h"

#include <json/json.hpp>

namespace EngineCore::Scripts {
ElectricBeamComponentLuaProxy::ElectricBeamComponentLuaProxy(const std::shared_ptr<BeamComponentBase>& baseComponent)
    : PrimitiveComponentLuaProxy(baseComponent)
{
}

ElectricBeamComponentLuaProxy::~ElectricBeamComponentLuaProxy()
{
}

bool ElectricBeamComponentLuaProxy::InvokeFunction(const std::string& functionName, const std::string& jsonParameters)
{
    const bool baseInvokeResult = PrimitiveComponentLuaProxy::InvokeFunction(functionName, jsonParameters);
    bool result = false;

    if (!baseInvokeResult) {
        if (const auto ownerComponentSp = mOwnerComponentWp.lock()) {
            const auto electricBeamComponentSp = std::dynamic_pointer_cast<BeamComponentBase>(ownerComponentSp);
            if (electricBeamComponentSp) {
                if ("SetStartWorldPosition" == functionName) {
                    const auto json = nlohmann::json::parse(jsonParameters);
                    if (json.contains("position")) {
                        const glm::vec3 position = nlohmann_utilities::GetXyzFromJsonMap(json["position"]);
                        electricBeamComponentSp->SetStartWorldPosition(position);
                        result = true;
                    } else {
                        result = false;
                    }
                } else if ("SetEndWorldPosition" == functionName) {
                    const auto json = nlohmann::json::parse(jsonParameters);
                    if (json.contains("position")) {
                        const glm::vec3 position = nlohmann_utilities::GetXyzFromJsonMap(json["position"]);
                        electricBeamComponentSp->SetEndWorldPosition(position);
                        result = true;
                    } else {
                        result = false;
                    }
                } else if ("SetBeamThickness" == functionName) {
                    const auto json = nlohmann::json::parse(jsonParameters);
                    if (json.contains("thickness")) {
                        const float thickness = json["thickness"].get<float>();
                        electricBeamComponentSp->SetBeamThickness(thickness);
                        result = true;
                    } else {
                        result = false;
                    }
                } else if ("SetBeamCount" == functionName) {
                    const auto json = nlohmann::json::parse(jsonParameters);
                    if (json.contains("count")) {
                        const int32_t count = json["count"].get<int32_t>();
                        electricBeamComponentSp->SetBeamCount(count);
                        result = true;
                    } else {
                        result = false;
                    }
                } else if ("SetJitterAmount" == functionName) {
                    const auto json = nlohmann::json::parse(jsonParameters);
                    if (json.contains("amount")) {
                        const float amount = json["amount"].get<float>();
                        electricBeamComponentSp->SetJitterAmount(amount);
                        result = true;
                    } else {
                        result = false;
                    }
                } else if ("SetUpdateFrequency" == functionName) {
                    const auto json = nlohmann::json::parse(jsonParameters);
                    if (json.contains("frequency")) {
                        const float frequency = json["frequency"].get<float>();
                        electricBeamComponentSp->SetUpdateFrequency(frequency);
                        result = true;
                    } else {
                        result = false;
                    }
                } else if ("SetRadialSegments" == functionName) {
                    const auto json = nlohmann::json::parse(jsonParameters);
                    if (json.contains("radialSegments")) {
                        const int32_t radialSegments = json["radialSegments"].get<int32_t>();
                        electricBeamComponentSp->SetRadialSegments(radialSegments);
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
