#include "UiScrollListLuaProxy.h"

#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/GUI/UiElements/UiScrollList.h"
#include "Core/GameCore/Scene.h"
#include "Core/UtilityCore/EngineMath.h"

#include <json/json.hpp>

using namespace EngineCore;
using namespace EngineCore::GUI;

namespace EngineCore {
namespace Scripts {

UiScrollListLuaProxy::UiScrollListLuaProxy(const std::shared_ptr<::EngineCore::GUI::UiScrollList>& ownerUiItem)
    : UiItemLuaProxy(ownerUiItem)
    , mSpacing(ownerUiItem->GetSpacing())
    , mScrollOffset(ownerUiItem->GetScrollOffset())
    , mScrollSpeed(ownerUiItem->GetScrollSpeed())
    , mScrollbarSide(static_cast<uint8_t>(ownerUiItem->GetScrollbarSide()))
    , mScrollbarBackgroundColor(ownerUiItem->GetScrollbarBackgroundColor())
    , mScrollbarThumbColor(ownerUiItem->GetScrollbarThumbColor())
    , mScrollbarThicknessPixels(ownerUiItem->GetScrollbarThicknessPixels())
{
}

void UiScrollListLuaProxy::SetSpacing_FromGameThread(const uint32_t value)
{
    if (mSpacing != value) {
        mSpacing = value;
        mIsLuaDataDirty = true;
    }
}

void UiScrollListLuaProxy::SetScrollOffset_FromGameThread(const int32_t value)
{
    if (mScrollOffset != value) {
        mScrollOffset = value;
        mIsLuaDataDirty = true;
    }
}

void UiScrollListLuaProxy::SetScrollSpeed_FromGameThread(const int32_t value)
{
    if (mScrollSpeed != value) {
        mScrollSpeed = value;
        mIsLuaDataDirty = true;
    }
}

void UiScrollListLuaProxy::SetScrollbarSide_FromGameThread(const uint8_t value)
{
    if (mScrollbarSide != value) {
        mScrollbarSide = value;
        mIsLuaDataDirty = true;
    }
}

void UiScrollListLuaProxy::SetScrollbarBackgroundColor_FromGameThread(const glm::vec3& value)
{
    if (!EngineMath::CheckSimilarityVec3(mScrollbarBackgroundColor, value)) {
        mScrollbarBackgroundColor = value;
        mIsLuaDataDirty = true;
    }
}

void UiScrollListLuaProxy::SetScrollbarThumbColor_FromGameThread(const glm::vec3& value)
{
    if (!EngineMath::CheckSimilarityVec3(mScrollbarThumbColor, value)) {
        mScrollbarThumbColor = value;
        mIsLuaDataDirty = true;
    }
}

void UiScrollListLuaProxy::SetScrollbarThicknessPixels_FromGameThread(const uint32_t value)
{
    if (mScrollbarThicknessPixels != value) {
        mScrollbarThicknessPixels = value;
        mIsLuaDataDirty = true;
    }
}

void UiScrollListLuaProxy::OnLuaThreadDataUpdated(const std::string& jsonParameters)
{
    static constexpr auto functionId = Hash64_CT("UiScrollListLuaProxy::OnLuaThreadDataUpdated");
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
                ext_assert(replicator, "UiScrollListLuaProxy::OnLuaThreadDataUpdated: replicator is null");
                const auto& scrollList = std::static_pointer_cast<::EngineCore::GUI::UiScrollList>(replicator);
                ext_assert(scrollList, "UiScrollListLuaProxy::OnLuaThreadDataUpdated: scrollList is null");
                scrollList->SyncFromLuaJsonProperties(jsonStr);
            });
    }
}

std::string UiScrollListLuaProxy::GetGameThreadData()
{
    const auto& baseJsonStr = UiItemBaseLuaProxy::GetGameThreadData();
    auto jsonObj = nlohmann::json::parse(baseJsonStr);
    jsonObj["spacing"] = mSpacing;
    jsonObj["scroll_offset"] = mScrollOffset;
    jsonObj["scroll_speed"] = mScrollSpeed;
    jsonObj["scrollbar_side"] = mScrollbarSide;
    jsonObj["scrollbar_background_color"]
        = std::vector<float>{mScrollbarBackgroundColor.r, mScrollbarBackgroundColor.g, mScrollbarBackgroundColor.b};
    jsonObj["scrollbar_thumb_color"] = std::vector<float>{mScrollbarThumbColor.r, mScrollbarThumbColor.g, mScrollbarThumbColor.b};
    jsonObj["scrollbar_thickness"] = mScrollbarThicknessPixels;
    return jsonObj.dump();
}

} // namespace Scripts
} // namespace EngineCore
