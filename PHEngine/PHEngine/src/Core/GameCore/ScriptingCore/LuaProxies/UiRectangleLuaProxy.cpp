#include "UiRectangleLuaProxy.h"
#include "Core/GameCore/GUI/UiElements/UiRectangle.h"
#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/LoggerExtension.h"

#include <json/json.hpp>

using namespace EngineCore;
using namespace EngineCore::GUI;

namespace EngineCore
{
    namespace Scripts
    {
        UiRectangleLuaProxy::UiRectangleLuaProxy(const std::shared_ptr<::EngineCore::GUI::UiRectangle> &ownerRectangle)
            : UiItemBaseLuaProxy(ownerRectangle),
              mColor(ownerRectangle->GetColor()),
              mOpacity(ownerRectangle->GetOpacity())
        {
        }

        void UiRectangleLuaProxy::OnLuaThreadDataUpdated(const std::string &jsonParameters)
        {
            static constexpr auto functionId = Hash64_CT("UiRectangleLuaProxy::OnLuaThreadDataUpdated");
            if (const auto sceneSp = mSceneWp.lock())
            {
                const auto replicatorId = GetReplicatorId();
                sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(
                    eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, mLuaProxyId, functionId, [sceneSp, replicatorId, jsonStr = jsonParameters]()
                    {
                    const auto &replicator = sceneSp->GetEngineToLuaReplicatorById(replicatorId);
                    assert(replicator);
                    const auto & uiRectangle = std::static_pointer_cast<::EngineCore::GUI::UiRectangle>(replicator);
                    assert(uiRectangle);
                    uiRectangle->SyncFromLuaJsonProperties(jsonStr); });
            }
        }

        std::string UiRectangleLuaProxy::GetGameThreadData()
        {
            const auto &baseJsonStr = UiItemBaseLuaProxy::GetGameThreadData();
            auto jsonObj = nlohmann::json::parse(baseJsonStr);
            std::vector<float> colorVec = {mColor.r, mColor.g, mColor.b};
            jsonObj["color"] = colorVec;
            jsonObj["opacity"] = mOpacity;
            return jsonObj.dump();
        }

        void UiRectangleLuaProxy::SetColor_FromGameThread(const glm::vec3 &color)
        {
            if (!EngineMath::CheckSimilarityVec3(mColor, color))
            {
                mColor = color;
                mIsLuaDataDirty = true;
            }
        }

        void UiRectangleLuaProxy::SetOpacity_FromGameThread(const float opacity)
        {
            if (!EngineMath::FloatsNearEqual(mOpacity, opacity))
            {
                mOpacity = opacity;
                mIsLuaDataDirty = true;
            }
        }
    }
}
