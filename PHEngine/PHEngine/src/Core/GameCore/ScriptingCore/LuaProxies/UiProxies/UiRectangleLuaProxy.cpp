#include "UiRectangleLuaProxy.h"

#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/GUI/UiElements/UiRectangle.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Scene.h"

#include <json/json.hpp>

using namespace EngineCore;
using namespace EngineCore::GUI;

namespace EngineCore {
namespace Scripts {
UiRectangleLuaProxy::UiRectangleLuaProxy(const std::shared_ptr<::EngineCore::GUI::UiRectangle>& ownerRectangle)
    : UiItemBaseLuaProxy(ownerRectangle)
    , mColor(ownerRectangle->GetColor())
    , mOpacity(ownerRectangle->GetOpacity())
    , mBorderRadius(static_cast<float>(ownerRectangle->GetBorderRadius()))
    , mIsRoundTop(ownerRectangle->GetIsRoundTop())
    , mIsRoundBottom(ownerRectangle->GetIsRoundBottom())
{
}

void UiRectangleLuaProxy::OnLuaThreadDataUpdated(const std::string& jsonParameters)
{
    static constexpr auto functionId = Hash64_CT("UiRectangleLuaProxy::OnLuaThreadDataUpdated");
    if (const auto sceneSp = mSceneWp.lock()) {
        const auto replicatorId = GetReplicatorId();
        sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(
            eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
            mLuaProxyId,
            functionId,
            [sceneSp, replicatorId, jsonStr = jsonParameters](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                const auto& replicator = sceneSp->GetEngineToLuaReplicatorById(replicatorId);
                ext_assert(replicator, "UiRectangleLuaProxy::OnLuaThreadDataUpdated: replicator is null");
                const auto& uiRectangle = std::static_pointer_cast<::EngineCore::GUI::UiRectangle>(replicator);
                ext_assert(uiRectangle, "UiRectangleLuaProxy::OnLuaThreadDataUpdated: uiRectangle is null");
                uiRectangle->SyncFromLuaJsonProperties(jsonStr);
            });
    }
}

std::string UiRectangleLuaProxy::GetGameThreadData()
{
    const auto& baseJsonStr = UiItemBaseLuaProxy::GetGameThreadData();
    auto jsonObj = nlohmann::json::parse(baseJsonStr);
    std::vector<float> colorVec = {mColor.r, mColor.g, mColor.b};
    jsonObj["color"] = colorVec;
    jsonObj["opacity"] = mOpacity;
    jsonObj["border_radius"] = mBorderRadius;
    jsonObj["is_round_top"] = mIsRoundTop;
    jsonObj["is_round_bottom"] = mIsRoundBottom;
    return jsonObj.dump();
}

void UiRectangleLuaProxy::SetColor_FromGameThread(const glm::vec3& color)
{
    if (!EngineMath::CheckSimilarityVec3(mColor, color)) {
        mColor = color;
        mIsLuaDataDirty = true;
    }
}

void UiRectangleLuaProxy::SetOpacity_FromGameThread(const float opacity)
{
    if (!EngineMath::FloatsNearEqual(mOpacity, opacity)) {
        mOpacity = opacity;
        mIsLuaDataDirty = true;
    }
}

void UiRectangleLuaProxy::SetBorderRadius_FromGameThread(const float borderRadius)
{
    if (!EngineMath::FloatsNearEqual(mBorderRadius, borderRadius)) {
        mBorderRadius = borderRadius;
        mIsLuaDataDirty = true;
    }
}

void UiRectangleLuaProxy::SetIsRoundTop_FromGameThread(const bool bIsRoundTop)
{
    if (mIsRoundTop != bIsRoundTop) {
        mIsRoundTop = bIsRoundTop;
        mIsLuaDataDirty = true;
    }
}

void UiRectangleLuaProxy::SetIsRoundBottom_FromGameThread(const bool bIsRoundBottom)
{
    if (mIsRoundBottom != bIsRoundBottom) {
        mIsRoundBottom = bIsRoundBottom;
        mIsLuaDataDirty = true;
    }
}
} // namespace Scripts
} // namespace EngineCore
