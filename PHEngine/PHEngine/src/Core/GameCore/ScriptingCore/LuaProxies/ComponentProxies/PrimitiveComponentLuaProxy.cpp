#include "PrimitiveComponentLuaProxy.h"

#include "Core/UtilityCore/JsonUtilities.h"

#include <json/json.hpp>

namespace EngineCore::Scripts {
PrimitiveComponentLuaProxy::PrimitiveComponentLuaProxy(const std::shared_ptr<PrimitiveComponent>& baseComponent)
    : SceneComponentLuaProxy(baseComponent)
{
}

PrimitiveComponentLuaProxy::~PrimitiveComponentLuaProxy()
{
}

bool PrimitiveComponentLuaProxy::InvokeFunction(const std::string& functionName, const std::string& jsonParameters)
{
    const bool baseInvokeResult = SceneComponentLuaProxy::InvokeFunction(functionName, jsonParameters);
    bool result = false;

    if (!baseInvokeResult) {
        if (const auto ownerComponentSp = mOwnerComponentWp.lock()) {
            const auto primitiveComponentSp = std::dynamic_pointer_cast<PrimitiveComponent>(ownerComponentSp);
            if (primitiveComponentSp) {
                if ("SetIsVisible" == functionName) {
                    const auto json = nlohmann::json::parse(jsonParameters);
                    if (json.contains("isVisible")) {
                        const bool isVisible = json["isVisible"].get<bool>();
                        primitiveComponentSp->SetIsVisible(isVisible);
                        result = true;
                    } else {
                        result = false;
                    }
                } else if ("SetSortOrderValue" == functionName) {
                    const auto json = nlohmann::json::parse(jsonParameters);
                    if (json.contains("orderValue")) {
                        const int32_t orderValue = json["orderValue"].get<int32_t>();
                        primitiveComponentSp->SetSortOrderValue(orderValue);
                        result = true;
                    } else {
                        result = false;
                    }
                } else if ("SetCanBloomBeApplied" == functionName) {
                    const auto json = nlohmann::json::parse(jsonParameters);
                    if (json.contains("value")) {
                        const bool value = json["value"].get<bool>();
                        primitiveComponentSp->SetCanBloomBeApplied(value);
                        result = true;
                    } else {
                        result = false;
                    }
                } else if ("SetDepthWriteMaskEnabled" == functionName) {
                    const auto json = nlohmann::json::parse(jsonParameters);
                    if (json.contains("isEnabled")) {
                        const bool isEnabled = json["isEnabled"].get<bool>();
                        primitiveComponentSp->SetDepthWriteMaskEnabled(isEnabled);
                        result = true;
                    } else {
                        result = false;
                    }
                } else if ("SetIsOutlineApplied" == functionName) {
                    const auto json = nlohmann::json::parse(jsonParameters);
                    if (json.contains("value")) {
                        const bool value = json["value"].get<bool>();
                        primitiveComponentSp->SetIsOutlineApplied(value);
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
