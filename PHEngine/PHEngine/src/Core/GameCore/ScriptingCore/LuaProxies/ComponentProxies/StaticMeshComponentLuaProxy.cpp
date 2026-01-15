#include "StaticMeshComponentLuaProxy.h"

#include "Core/UtilityCore/JsonUtilities.h"

#include <json/json.hpp>

namespace EngineCore::Scripts {
StaticMeshComponentLuaProxy::StaticMeshComponentLuaProxy(const std::shared_ptr<StaticMeshComponent>& baseComponent)
    : PrimitiveComponentLuaProxy(baseComponent)
{
}

StaticMeshComponentLuaProxy::~StaticMeshComponentLuaProxy()
{
}

bool StaticMeshComponentLuaProxy::InvokeFunction(const std::string& functionName, const std::string& jsonParameters)
{
    const bool baseInvokeResult = PrimitiveComponentLuaProxy::InvokeFunction(functionName, jsonParameters);
    bool result = false;

    if (!baseInvokeResult) {
        if (const auto ownerComponentSp = mOwnerComponentWp.lock()) {
            const auto staticMeshComponentSp = std::dynamic_pointer_cast<StaticMeshComponent>(ownerComponentSp);
            if (staticMeshComponentSp) {
                if ("SetMeshModelPath" == functionName) {
                    const auto json = nlohmann::json::parse(jsonParameters);
                    if (json.contains("modelPath")) {
                        const std::string modelPath = json["modelPath"].get<std::string>();
                        staticMeshComponentSp->SetMeshModelPath(modelPath);
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
