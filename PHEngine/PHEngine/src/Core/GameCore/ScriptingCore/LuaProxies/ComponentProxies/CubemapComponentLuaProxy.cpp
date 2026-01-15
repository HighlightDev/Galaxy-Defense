#include "CubemapComponentLuaProxy.h"

#include "Core/UtilityCore/JsonUtilities.h"

#include <json/json.hpp>

namespace EngineCore::Scripts {
CubemapComponentLuaProxy::CubemapComponentLuaProxy(const std::shared_ptr<CubemapComponent>& baseComponent)
    : PrimitiveComponentLuaProxy(baseComponent)
{
}

CubemapComponentLuaProxy::~CubemapComponentLuaProxy()
{
}

bool CubemapComponentLuaProxy::InvokeFunction(const std::string& functionName, const std::string& jsonParameters)
{
    const bool baseInvokeResult = PrimitiveComponentLuaProxy::InvokeFunction(functionName, jsonParameters);
    bool result = false;

    if (!baseInvokeResult) {
        if (const auto ownerComponentSp = mOwnerComponentWp.lock()) {
            const auto cubemapComponentSp = std::dynamic_pointer_cast<CubemapComponent>(ownerComponentSp);
            if (cubemapComponentSp) {
                // Currently no specific methods to expose for CubemapComponent
                // All functionality is inherited from PrimitiveComponentLuaProxy
            }
        }
    }
    return result || baseInvokeResult;
}
} // namespace EngineCore::Scripts
