#include "UiScrollListLuaProxy.h"

#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/GUI/UiElements/UiScrollList.h"
#include "Core/GameCore/Scene.h"

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
    return jsonObj.dump();
}

} // namespace Scripts
} // namespace EngineCore
