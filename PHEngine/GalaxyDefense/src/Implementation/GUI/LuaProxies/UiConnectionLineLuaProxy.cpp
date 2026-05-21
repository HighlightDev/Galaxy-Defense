#include "UiConnectionLineLuaProxy.h"

#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/Scene.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Implementation/GUI/UiConnectionLine.h"

#include <json/json.hpp>

using namespace EngineCore;
using namespace EngineCore::GUI;
using namespace EngineCore::Scripts;

namespace Game {

namespace {
bool Vec2NearEqual(const glm::vec2& a, const glm::vec2& b)
{
    return EngineMath::FloatsNearEqual(a.x, b.x) && EngineMath::FloatsNearEqual(a.y, b.y);
}
} // namespace

UiConnectionLineLuaProxy::UiConnectionLineLuaProxy(const std::shared_ptr<UiConnectionLine>& ownerUiItem)
    : UiItemBaseLuaProxy(ownerUiItem)
    , mStartPoint(ownerUiItem->GetStartPoint())
    , mEndPoint(ownerUiItem->GetEndPoint())
    , mStartAnchorTarget(ownerUiItem->GetStartAnchorTarget())
    , mEndAnchorTarget(ownerUiItem->GetEndAnchorTarget())
    , mColor(ownerUiItem->GetColor())
    , mThicknessPx(ownerUiItem->GetThicknessPx())
    , mDashLengthPx(ownerUiItem->GetDashLengthPx())
    , mGapLengthPx(ownerUiItem->GetGapLengthPx())
    , mOpacity(ownerUiItem->GetOpacity())
{
}

void UiConnectionLineLuaProxy::OnLuaThreadDataUpdated(const std::string& jsonParameters)
{
    static constexpr auto functionId = Hash64_CT("UiConnectionLineLuaProxy::OnLuaThreadDataUpdated");
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
                ext_assert(replicator, "UiConnectionLineLuaProxy::OnLuaThreadDataUpdated: replicator is null");
                const auto& uiConnectionLine = std::static_pointer_cast<UiConnectionLine>(replicator);
                ext_assert(uiConnectionLine, "UiConnectionLineLuaProxy::OnLuaThreadDataUpdated: uiConnectionLine is null");
                uiConnectionLine->SyncFromLuaJsonProperties(jsonStr);
            });
    }
}

std::string UiConnectionLineLuaProxy::GetGameThreadData()
{
    const auto& baseJsonStr = UiItemBaseLuaProxy::GetGameThreadData();
    auto jsonObj = nlohmann::json::parse(baseJsonStr);
    jsonObj["start_point"] = {{"x", mStartPoint.x}, {"y", mStartPoint.y}};
    jsonObj["end_point"] = {{"x", mEndPoint.x}, {"y", mEndPoint.y}};
    jsonObj["start_anchor_target"] = mStartAnchorTarget;
    jsonObj["end_anchor_target"] = mEndAnchorTarget;
    jsonObj["color"] = {{"r", mColor.r}, {"g", mColor.g}, {"b", mColor.b}};
    jsonObj["thickness_px"] = mThicknessPx;
    jsonObj["dash_length_px"] = mDashLengthPx;
    jsonObj["gap_length_px"] = mGapLengthPx;
    jsonObj["opacity"] = mOpacity;
    return jsonObj.dump();
}

void UiConnectionLineLuaProxy::SetStartPoint_FromGameThread(const glm::vec2& startPoint)
{
    if (!Vec2NearEqual(mStartPoint, startPoint)) {
        mStartPoint = startPoint;
        mIsLuaDataDirty = true;
    }
}

void UiConnectionLineLuaProxy::SetEndPoint_FromGameThread(const glm::vec2& endPoint)
{
    if (!Vec2NearEqual(mEndPoint, endPoint)) {
        mEndPoint = endPoint;
        mIsLuaDataDirty = true;
    }
}

void UiConnectionLineLuaProxy::SetStartAnchorTarget_FromGameThread(const std::string& targetUiItemName)
{
    if (mStartAnchorTarget != targetUiItemName) {
        mStartAnchorTarget = targetUiItemName;
        mIsLuaDataDirty = true;
    }
}

void UiConnectionLineLuaProxy::SetEndAnchorTarget_FromGameThread(const std::string& targetUiItemName)
{
    if (mEndAnchorTarget != targetUiItemName) {
        mEndAnchorTarget = targetUiItemName;
        mIsLuaDataDirty = true;
    }
}

void UiConnectionLineLuaProxy::SetColor_FromGameThread(const glm::vec3& color)
{
    if (!EngineMath::CheckSimilarityVec3(mColor, color)) {
        mColor = color;
        mIsLuaDataDirty = true;
    }
}

void UiConnectionLineLuaProxy::SetThicknessPx_FromGameThread(const float thicknessPx)
{
    if (!EngineMath::FloatsNearEqual(mThicknessPx, thicknessPx)) {
        mThicknessPx = thicknessPx;
        mIsLuaDataDirty = true;
    }
}

void UiConnectionLineLuaProxy::SetDashLengthPx_FromGameThread(const float dashLengthPx)
{
    if (!EngineMath::FloatsNearEqual(mDashLengthPx, dashLengthPx)) {
        mDashLengthPx = dashLengthPx;
        mIsLuaDataDirty = true;
    }
}

void UiConnectionLineLuaProxy::SetGapLengthPx_FromGameThread(const float gapLengthPx)
{
    if (!EngineMath::FloatsNearEqual(mGapLengthPx, gapLengthPx)) {
        mGapLengthPx = gapLengthPx;
        mIsLuaDataDirty = true;
    }
}

void UiConnectionLineLuaProxy::SetOpacity_FromGameThread(const float opacity)
{
    if (!EngineMath::FloatsNearEqual(mOpacity, opacity)) {
        mOpacity = opacity;
        mIsLuaDataDirty = true;
    }
}

} // namespace Game
