#include "UiProgressBarLuaProxy.h"

#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/GUI/UiElements/UiProgressBar.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Scene.h"

#include <json/json.hpp>

using namespace EngineCore;
using namespace EngineCore::GUI;

namespace EngineCore::Scripts {
UiProgressBarLuaProxy::UiProgressBarLuaProxy(const std::shared_ptr<::EngineCore::GUI::UiProgressBar>& ownerProgressBar)
    : UiItemBaseLuaProxy(ownerProgressBar)
    , mEmptyColor(ownerProgressBar->GetEmptyColor())
    , mFilledColor(ownerProgressBar->GetFilledColor())
    , mOpacity(ownerProgressBar->GetOpacity())
    , mFillPercentValue(ownerProgressBar->GetFillPercentValue())
{
}

void UiProgressBarLuaProxy::OnLuaThreadDataUpdated(const std::string& jsonParameters)
{
    static constexpr auto functionId = Hash64_CT("UiProgressBarLuaProxy::OnLuaThreadDataUpdated");
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
                ext_assert(replicator, "UiProgressBarLuaProxy::OnLuaThreadDataUpdated: replicator is null");
                const auto& uiProgressBar = std::static_pointer_cast<::EngineCore::GUI::UiProgressBar>(replicator);
                ext_assert(uiProgressBar, "UiProgressBarLuaProxy::OnLuaThreadDataUpdated: uiProgressBar is null");
                uiProgressBar->SyncFromLuaJsonProperties(jsonStr);
            });
    }
}

std::string UiProgressBarLuaProxy::GetGameThreadData()
{
    const auto& baseJsonStr = UiItemBaseLuaProxy::GetGameThreadData();
    auto jsonObj = nlohmann::json::parse(baseJsonStr);
    std::vector<float> emptyColorVec = {mEmptyColor.r, mEmptyColor.g, mEmptyColor.b};
    std::vector<float> filledColorVec = {mFilledColor.r, mFilledColor.g, mFilledColor.b};
    jsonObj["empty_color"] = emptyColorVec;
    jsonObj["filled_color"] = filledColorVec;
    jsonObj["opacity"] = mOpacity;
    jsonObj["fill_percent_value"] = mFillPercentValue;
    jsonObj["border_radius"] = mBorderRadius;
    return jsonObj.dump();
}

void UiProgressBarLuaProxy::SetEmptyColor_FromGameThread(const glm::vec3& color)
{
    if (!EngineMath::CheckSimilarityVec3(mEmptyColor, color)) {
        mEmptyColor = color;
        mIsLuaDataDirty = true;
    }
}

void UiProgressBarLuaProxy::SetFilledColor_FromGameThread(const glm::vec3& color)
{
    if (!EngineMath::CheckSimilarityVec3(mFilledColor, color)) {
        mFilledColor = color;
        mIsLuaDataDirty = true;
    }
}

void UiProgressBarLuaProxy::SetOpacity_FromGameThread(const float opacity)
{
    if (!EngineMath::FloatsNearEqual(mOpacity, opacity)) {
        mOpacity = opacity;
        mIsLuaDataDirty = true;
    }
}

void UiProgressBarLuaProxy::SetFillPercentValue_FromGrameThread(const float fillPercentValue)
{
    if (!EngineMath::FloatsNearEqual(mFillPercentValue, fillPercentValue)) {
        mFillPercentValue = fillPercentValue;
        mIsLuaDataDirty = true;
    }
}

void UiProgressBarLuaProxy::SetBorderRadius_FromGameThread(const float borderRadius)
{
    if (!EngineMath::FloatsNearEqual(mBorderRadius, borderRadius)) {
        mBorderRadius = borderRadius;
        mIsLuaDataDirty = true;
    }
}
} // namespace EngineCore::Scripts
