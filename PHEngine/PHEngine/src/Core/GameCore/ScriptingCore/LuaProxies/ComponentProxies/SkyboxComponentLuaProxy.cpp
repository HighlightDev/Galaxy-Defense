#include "SkyboxComponentLuaProxy.h"

#include "Core/UtilityCore/JsonUtilities.h"

#include <json/json.hpp>

namespace EngineCore::Scripts {
SkyboxComponentLuaProxy::SkyboxComponentLuaProxy(const std::shared_ptr<SkyboxComponent>& baseComponent)
    : PrimitiveComponentLuaProxy(baseComponent)
{
}

SkyboxComponentLuaProxy::~SkyboxComponentLuaProxy()
{
}

bool SkyboxComponentLuaProxy::InvokeFunction(const std::string& functionName, const std::string& jsonParameters)
{
    const bool baseInvokeResult = PrimitiveComponentLuaProxy::InvokeFunction(functionName, jsonParameters);
    bool result = false;

    if (!baseInvokeResult) {
        if (const auto ownerComponentSp = mOwnerComponentWp.lock()) {
            const auto skyboxComponentSp = std::dynamic_pointer_cast<SkyboxComponent>(ownerComponentSp);
            if (skyboxComponentSp) {
                if ("SetRotateSpeed" == functionName) {
                    const auto json = nlohmann::json::parse(jsonParameters);
                    if (json.contains("rotateSpeed")) {
                        const float rotateSpeed = json["rotateSpeed"].get<float>();
                        skyboxComponentSp->SetRotateSpeed(rotateSpeed);
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
