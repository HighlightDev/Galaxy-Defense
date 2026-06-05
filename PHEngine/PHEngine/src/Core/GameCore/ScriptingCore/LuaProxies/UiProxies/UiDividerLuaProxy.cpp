#include "UiDividerLuaProxy.h"

#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/GUI/UiElements/UiDivider.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Scene.h"

#include <json/json.hpp>

using namespace EngineCore;
using namespace EngineCore::GUI;

namespace EngineCore {
namespace Scripts {
UiDividerLuaProxy::UiDividerLuaProxy(const std::shared_ptr<::EngineCore::GUI::UiDivider>& owner)
    : UiItemBaseLuaProxy(owner)
    , mColor(owner->GetColor())
    , mOpacity(owner->GetOpacity())
    , mLineWidthPx(owner->GetLineWidthPx())
    , mEdgeFade(owner->GetEdgeFade())
    , mOrientation(owner->GetDividerOrientation())
{
}

void UiDividerLuaProxy::OnLuaThreadDataUpdated(const std::string& jsonParameters)
{
    static constexpr auto functionId = Hash64_CT("UiDividerLuaProxy::OnLuaThreadDataUpdated");
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
                ext_assert(replicator, "UiDividerLuaProxy::OnLuaThreadDataUpdated: replicator is null");
                const auto& uiDivider = std::static_pointer_cast<::EngineCore::GUI::UiDivider>(replicator);
                ext_assert(uiDivider, "UiDividerLuaProxy::OnLuaThreadDataUpdated: uiDivider is null");
                uiDivider->SyncFromLuaJsonProperties(jsonStr);
            });
    }
}

std::string UiDividerLuaProxy::GetGameThreadData()
{
    const auto& baseJsonStr = UiItemBaseLuaProxy::GetGameThreadData();
    auto jsonObj = nlohmann::json::parse(baseJsonStr);
    std::vector<float> colorVec = {mColor.r, mColor.g, mColor.b};
    jsonObj["color"] = colorVec;
    jsonObj["opacity"] = mOpacity;
    jsonObj["line_width_px"] = mLineWidthPx;
    jsonObj["edge_fade"] = mEdgeFade;
    jsonObj["divider_orientation"] = static_cast<int32_t>(mOrientation);
    return jsonObj.dump();
}

void UiDividerLuaProxy::SetColor_FromGameThread(const glm::vec3& color)
{
    if (!EngineMath::CheckSimilarityVec3(mColor, color)) {
        mColor = color;
        mIsLuaDataDirty = true;
    }
}

void UiDividerLuaProxy::SetOpacity_FromGameThread(const float opacity)
{
    if (!EngineMath::FloatsNearEqual(mOpacity, opacity)) {
        mOpacity = opacity;
        mIsLuaDataDirty = true;
    }
}

void UiDividerLuaProxy::SetLineWidthPx_FromGameThread(const float widthPx)
{
    if (!EngineMath::FloatsNearEqual(mLineWidthPx, widthPx)) {
        mLineWidthPx = widthPx;
        mIsLuaDataDirty = true;
    }
}

void UiDividerLuaProxy::SetEdgeFade_FromGameThread(const float edgeFade)
{
    if (!EngineMath::FloatsNearEqual(mEdgeFade, edgeFade)) {
        mEdgeFade = edgeFade;
        mIsLuaDataDirty = true;
    }
}

void UiDividerLuaProxy::SetDividerOrientation_FromGameThread(const eDividerOrientation orientation)
{
    if (mOrientation != orientation) {
        mOrientation = orientation;
        mIsLuaDataDirty = true;
    }
}
} // namespace Scripts
} // namespace EngineCore
