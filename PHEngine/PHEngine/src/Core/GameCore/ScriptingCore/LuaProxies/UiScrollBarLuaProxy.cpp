#include "UiScrollBarLuaProxy.h"

#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/GUI/UiElements/UiScrollBar.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Scene.h"

#include <json/json.hpp>

using namespace EngineCore;
using namespace EngineCore::GUI;

namespace EngineCore {
namespace Scripts {
UiScrollBarLuaProxy::UiScrollBarLuaProxy(const std::shared_ptr<UiScrollBar>& ownerScrollBar)
    : UiItemBaseLuaProxy(ownerScrollBar)
    , mMaxScrollValue(ownerScrollBar->GetMaxScrollValue())
    , mMinScrollValue(ownerScrollBar->GetMinScrollValue())
    , mScrollValue(ownerScrollBar->GetScrollValue())
    , mScrollStep(ownerScrollBar->GetScrollStep())
{
}

void UiScrollBarLuaProxy::OnLuaThreadDataUpdated(const std::string& jsonParameters)
{
    static constexpr auto functionId = Hash64_CT("UiScrollBarLuaProxy::OnLuaThreadDataUpdated");
    if (const auto sceneSp = mSceneWp.lock()) {
        const auto replicatorId = GetReplicatorId();
        sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(
            eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
            mLuaProxyId,
            functionId,
            [sceneSp, replicatorId, jsonStr = jsonParameters]() {
                const auto& replicator = sceneSp->GetEngineToLuaReplicatorById(replicatorId);
                assert(replicator);
                const auto& uiRectangle = std::static_pointer_cast<::EngineCore::GUI::UiRectangle>(replicator);
                assert(uiRectangle);
                uiRectangle->SyncFromLuaJsonProperties(jsonStr);
            });
    }
}

std::string UiScrollBarLuaProxy::GetGameThreadData()
{
    const auto& baseJsonStr = UiItemBaseLuaProxy::GetGameThreadData();
    auto jsonObj = nlohmann::json::parse(baseJsonStr);
    jsonObj["max_scroll_value"] = mMaxScrollValue;
    jsonObj["min_scroll_value"] = mMinScrollValue;
    jsonObj["scroll_value"] = mScrollValue;
    jsonObj["scroll_step"] = mScrollStep;
    return jsonObj.dump();
}

void UiScrollBarLuaProxy::SetMaxScrollValue_FromGameThread(const float maxScrollValue)
{
    mMaxScrollValue = maxScrollValue;
}

void UiScrollBarLuaProxy::SetMinScrollValue_FromGameThread(const float minScrollValue)
{
    mMinScrollValue = minScrollValue;
}

void UiScrollBarLuaProxy::SetScrollValue_FromGameThread(const float scrollValue)
{
    mScrollValue = scrollValue;
}

void UiScrollBarLuaProxy::SetScrollStep_FromGameThread(const float scrollStep)
{
    mScrollStep = scrollStep;
}
} // namespace Scripts
} // namespace EngineCore
