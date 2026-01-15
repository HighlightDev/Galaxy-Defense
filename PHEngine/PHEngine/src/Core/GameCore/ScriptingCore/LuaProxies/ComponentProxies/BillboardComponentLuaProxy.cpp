#include "BillboardComponentLuaProxy.h"

#include "Core/UtilityCore/JsonUtilities.h"

#include <json/json.hpp>

namespace EngineCore::Scripts {
BillboardComponentLuaProxy::BillboardComponentLuaProxy(const std::shared_ptr<BillboardComponent>& baseComponent)
    : PrimitiveComponentLuaProxy(baseComponent)
{
}

BillboardComponentLuaProxy::~BillboardComponentLuaProxy()
{
}

bool BillboardComponentLuaProxy::InvokeFunction(const std::string& functionName, const std::string& jsonParameters)
{
    const bool baseInvokeResult = PrimitiveComponentLuaProxy::InvokeFunction(functionName, jsonParameters);
    bool result = false;

    if (!baseInvokeResult) {
        if (const auto ownerComponentSp = mOwnerComponentWp.lock()) {
            const auto billboardComponentSp = std::dynamic_pointer_cast<BillboardComponent>(ownerComponentSp);
            if (billboardComponentSp) {
                if ("SetBillboardExtent" == functionName) {
                    const auto json = nlohmann::json::parse(jsonParameters);
                    if (json.contains("extent")) {
                        const float extent = json["extent"].get<float>();
                        billboardComponentSp->SetBillboardExtent(extent);
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
