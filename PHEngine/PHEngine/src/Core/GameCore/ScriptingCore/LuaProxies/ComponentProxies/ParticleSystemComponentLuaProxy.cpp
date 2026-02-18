#include "ParticleSystemComponentLuaProxy.h"

#include "Core/UtilityCore/JsonUtilities.h"

#include <json/json.hpp>

namespace EngineCore::Scripts {
ParticleSystemComponentLuaProxy::ParticleSystemComponentLuaProxy(
    const std::shared_ptr<ParticleSystemBaseComponent>& baseComponent)
    : PrimitiveComponentLuaProxy(baseComponent)
{
}

ParticleSystemComponentLuaProxy::~ParticleSystemComponentLuaProxy()
{
}

bool ParticleSystemComponentLuaProxy::InvokeFunction(const std::string& functionName, const std::string& jsonParameters)
{
    const bool baseInvokeResult = PrimitiveComponentLuaProxy::InvokeFunction(functionName, jsonParameters);
    bool result = false;

    if (!baseInvokeResult) {
        if (const auto ownerComponentSp = mOwnerComponentWp.lock()) {
            const auto particleSystemComponentSp = std::dynamic_pointer_cast<ParticleSystemBaseComponent>(ownerComponentSp);
            if (particleSystemComponentSp) {
                if ("EmitParticles" == functionName) {
                    particleSystemComponentSp->EmitParticles();
                    result = true;
                } else if ("ResetParticles" == functionName) {
                    particleSystemComponentSp->ResetParticles();
                    result = true;
                }
            }
        }
    }
    return result || baseInvokeResult;
}
} // namespace EngineCore::Scripts
