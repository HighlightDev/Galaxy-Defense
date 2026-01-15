#include "BaseComponentLuaProxy.h"

#include "Core/GameCore/Components/Component.h"
#include "Core/UtilityCore/JsonUtilities.h"

#include <json/json.hpp>

using namespace EngineCore;

namespace EngineCore::Scripts {

BaseComponentLuaProxy::BaseComponentLuaProxy(const std::shared_ptr<Component>& ownerComponent)
    : LuaProxy()
    , mOwnerComponentWp(ownerComponent)
{
    mLuaProxyId = ownerComponent->GetLuaProxyId();
    SetReplicatorId(ownerComponent->GetReplicatorId());
}

bool BaseComponentLuaProxy::InvokeFunction(const std::string& functionName, const std::string& jsonParameters)
{
    bool result = false;
    if (const auto ownerComponentSp = mOwnerComponentWp.lock()) {
        if ("SetIsEnabled" == functionName) {
            const auto json = nlohmann::json::parse(jsonParameters);
            if (json.contains("value")) {
                const bool isEnabled = nlohmann_utilities::GetBoolFromJson(json, "value");
                ownerComponentSp->SetIsEnabled(isEnabled);
                result = true;
            } else {
                result = false;
            }
        }
    }
    return result;
}

void BaseComponentLuaProxy::OnLuaThreadDataUpdated(const std::string& jsonParameters)
{
    // Not implemented in base class
}

std::string BaseComponentLuaProxy::GetGameThreadData()
{
    // Not implemented in base class
    return "";
}
} // namespace EngineCore::Scripts
