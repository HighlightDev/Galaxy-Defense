#include "UiToggleButtonLuaProxy.h"
#include "Core/GameCore/GUI/UiElements/UiToggleButton.h"
#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/GUI/UiInputSystem/UiMouseInputReceiverToggleButton.h"

#include <json/json.hpp>

using namespace EngineCore;
using namespace EngineCore::GUI;

namespace EngineCore
{
    namespace Scripts
    {
        UiToggleButtonLuaProxy::UiToggleButtonLuaProxy(const std::shared_ptr<::EngineCore::GUI::UiToggleButton> &ownerButton)
            : UiItemBaseLuaProxy(ownerButton),
              mToggleOffColor(ownerButton->GetToggleOffColor()),
              mToggleOnColor(ownerButton->GetToggleOnColor()),
              mOpacity(ownerButton->GetOpacity()),
              mIsStateOn(ownerButton->IsButtonStateOn())
        {
        }

        void UiToggleButtonLuaProxy::OnLuaThreadDataUpdated(const std::string &jsonParameters)
        {
            static constexpr auto functionId = Hash64_CT("UiToggleButtonLuaProxy::OnLuaThreadDataUpdated");
            if (const auto sceneSp = mSceneWp.lock())
            {
                const auto replicatorId = GetReplicatorId();
                sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(
                    eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, mLuaProxyId, functionId, [sceneSp, replicatorId, jsonStr = jsonParameters]()
                    {
                    const auto &replicator = sceneSp->GetEngineToLuaReplicatorById(replicatorId);
                    assert(replicator);
                    const auto &uiToggleButton = std::static_pointer_cast<::EngineCore::GUI::UiToggleButton>(replicator);
                    assert(uiToggleButton);
                    uiToggleButton->SyncFromLuaJsonProperties(jsonStr); });
            }
        }

        std::string UiToggleButtonLuaProxy::GetGameThreadData()
        {
            const auto &baseJsonStr = UiItemBaseLuaProxy::GetGameThreadData();
            auto jsonObj = nlohmann::json::parse(baseJsonStr);
            const auto toggleOnColor = std::vector<float>({mToggleOnColor.r, mToggleOnColor.g, mToggleOnColor.b});
            const auto toggleOffColor = std::vector<float>({mToggleOffColor.r, mToggleOffColor.g, mToggleOffColor.b});
            jsonObj["toggle_on_color"] = toggleOnColor;
            jsonObj["toggle_off_color"] = toggleOffColor;
            jsonObj["opacity"] = mOpacity;
            jsonObj["is_state_on"] = mIsStateOn;
            return jsonObj.dump();
        }

        void UiToggleButtonLuaProxy::EnableMouseInputReceiver()
        {
            if (mIsMouseInputReceiverEnabled)
                return;

            static constexpr auto functionId = Hash64_CT("UiToggleButtonLuaProxy::EnableMouseInputReceiver");
            if (const auto sceneSp = mSceneWp.lock())
            {
                mIsMouseInputReceiverEnabled = true;
                const auto replicatorId = GetReplicatorId();
                sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(
                    eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, mLuaProxyId, functionId, [sceneSp, replicatorId]()
                    {
                    const auto &replicator = sceneSp->GetEngineToLuaReplicatorById(replicatorId);
                    assert(replicator);
                    const auto &uiToggleButton = std::static_pointer_cast<::EngineCore::GUI::UiToggleButton>(replicator);
                    assert(uiToggleButton);
                    assert(uiToggleButton->GetParentCanvas().lock());
                    uiToggleButton->SetMouseInputReceiver(std::make_shared<UiMouseInputReceiverToggleButton>(uiToggleButton)); });
            }
        }

        void UiToggleButtonLuaProxy::SetToggleOnColor_FromGameThread(const glm::vec3 &color)
        {
            if (!EngineMath::CheckSimilarityVec3(color, mToggleOnColor))
            {
                mToggleOnColor = color;
                mIsLuaDataDirty = true;
            }
        }

        void UiToggleButtonLuaProxy::SetToggleOffColor_FromGameThread(const glm::vec3 &color)
        {
            if (!EngineMath::CheckSimilarityVec3(color, mToggleOffColor))
            {
                mToggleOffColor = color;
                mIsLuaDataDirty = true;
            }
        }

        void UiToggleButtonLuaProxy::SetIsStateOn_FromGameThread(const bool isStateOn)
        {
            if (mIsStateOn != isStateOn)
            {
                mIsStateOn = isStateOn;
                mIsLuaDataDirty = true;
            }
        }

        void UiToggleButtonLuaProxy::SetOpacity_FromGameThread(const float opacity)
        {
            if (!EngineMath::FloatsNearEqual(mOpacity, opacity))
            {
                mOpacity = opacity;
                mIsLuaDataDirty = true;
            }
        }
    }
}
