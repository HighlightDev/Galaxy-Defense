#include "SkeletalMeshComponentLuaProxy.h"

#include "Core/UtilityCore/JsonUtilities.h"

#include <json/json.hpp>

namespace EngineCore::Scripts {
SkeletalMeshComponentLuaProxy::SkeletalMeshComponentLuaProxy(const std::shared_ptr<SkeletalMeshComponent>& baseComponent)
    : PrimitiveComponentLuaProxy(baseComponent)
{
}

SkeletalMeshComponentLuaProxy::~SkeletalMeshComponentLuaProxy()
{
}

bool SkeletalMeshComponentLuaProxy::InvokeFunction(const std::string& functionName, const std::string& jsonParameters)
{
    const bool baseInvokeResult = PrimitiveComponentLuaProxy::InvokeFunction(functionName, jsonParameters);
    bool result = false;

    if (!baseInvokeResult) {
        if (const auto ownerComponentSp = mOwnerComponentWp.lock()) {
            const auto skeletalMeshComponentSp = std::dynamic_pointer_cast<SkeletalMeshComponent>(ownerComponentSp);
            if (skeletalMeshComponentSp) {
                if ("SetMeshModelPath" == functionName) {
                    const auto json = nlohmann::json::parse(jsonParameters);
                    if (json.contains("modelPath")) {
                        const std::string modelPath = json["modelPath"].get<std::string>();
                        skeletalMeshComponentSp->SetMeshModelPath(modelPath);
                        result = true;
                    } else if ("SetTimeIncreaseMultiply" == functionName) {
                        const auto json = nlohmann::json::parse(jsonParameters);
                        if (json.contains("timeMultiply")) {
                            const float timeMultiply = nlohmann_utilities::GetFloatFromJson(json, "timeMultiply");
                            skeletalMeshComponentSp->SetTimeIncreaseMultiply(timeMultiply);
                            result = true;
                        } else {
                            result = false;
                        }
                    }
                }
            }
        }
    }
    return result || baseInvokeResult;
}
} // namespace EngineCore::Scripts
