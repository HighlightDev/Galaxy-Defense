#include "SoundComponentLuaProxy.h"

#include "Core/UtilityCore/JsonUtilities.h"

#include <json/json.hpp>

namespace EngineCore::Scripts {
SoundComponentLuaProxy::SoundComponentLuaProxy(const std::shared_ptr<SoundComponent>& baseComponent)
    : BaseComponentLuaProxy(baseComponent)
{
}

SoundComponentLuaProxy::~SoundComponentLuaProxy()
{
}

bool SoundComponentLuaProxy::InvokeFunction(const std::string& functionName, const std::string& jsonParameters)
{
    const bool baseInvokeResult = BaseComponentLuaProxy::InvokeFunction(functionName, jsonParameters);
    bool result = false;

    if (!baseInvokeResult) {
        if (const auto ownerComponentSp = mOwnerComponentWp.lock()) {
            const auto soundComponentSp = std::dynamic_pointer_cast<SoundComponent>(ownerComponentSp);
            if (soundComponentSp) {
                if ("PlayBuffer" == functionName) {
                    const auto json = nlohmann::json::parse(jsonParameters);
                    if (json.contains("soundName")) {
                        const std::string soundName = nlohmann_utilities::GetStringFromJson(json, "soundName");
                        soundComponentSp->PlayBuffer(soundName);
                        result = true;
                    } else {
                        result = false;
                    }
                } else if ("SetGain" == functionName) {
                    const auto json = nlohmann::json::parse(jsonParameters);
                    if (json.contains("gain")) {
                        const float gain = nlohmann_utilities::GetFloatFromJson(json, "gain");
                        soundComponentSp->SetGain(gain);
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