#include "InstancedStaticMeshComponentLuaProxy.h"

#include "Core/UtilityCore/JsonUtilities.h"

#include <json/json.hpp>

namespace EngineCore::Scripts {
InstancedStaticMeshComponentLuaProxy::InstancedStaticMeshComponentLuaProxy(
    const std::shared_ptr<InstancedStaticMeshComponent>& baseComponent)
    : PrimitiveComponentLuaProxy(baseComponent)
{
}

InstancedStaticMeshComponentLuaProxy::~InstancedStaticMeshComponentLuaProxy()
{
}

bool InstancedStaticMeshComponentLuaProxy::InvokeFunction(const std::string& functionName, const std::string& jsonParameters)
{
    const bool baseInvokeResult = PrimitiveComponentLuaProxy::InvokeFunction(functionName, jsonParameters);
    bool result = false;

    if (!baseInvokeResult) {
        if (const auto ownerComponentSp = mOwnerComponentWp.lock()) {
            const auto instancedMeshComponentSp = std::dynamic_pointer_cast<InstancedStaticMeshComponent>(ownerComponentSp);
            if (instancedMeshComponentSp) {
                // Currently no specific methods to expose for InstancedStaticMeshComponent
                // All functionality is inherited from PrimitiveComponentLuaProxy
            }
        }
    }
    return result || baseInvokeResult;
}
} // namespace EngineCore::Scripts
