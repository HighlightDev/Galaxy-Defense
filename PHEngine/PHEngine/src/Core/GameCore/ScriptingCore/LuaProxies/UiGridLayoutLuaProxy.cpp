#include "UiGridLayoutLuaProxy.h"

#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/GUI/UiElements/UiGridLayout.h"
#include "Core/GameCore/Scene.h"
#include "Core/UtilityCore/EngineMath.h"

#include <json/json.hpp>

using namespace EngineCore;
using namespace EngineCore::GUI;

namespace EngineCore {
namespace Scripts {
UiGridLayoutLuaProxy::UiGridLayoutLuaProxy(const std::shared_ptr<::EngineCore::GUI::UiGridLayout>& ownerUiItem)
    : UiItemLuaProxy(ownerUiItem)
    , mHorizontalSpacing(ownerUiItem->GetHorizontalSpacing())
    , mVerticalSpacing(ownerUiItem->GetVerticalSpacing())
    , mColumnsCount(ownerUiItem->GetColumnsCount())
    , mRowsCount(ownerUiItem->GetRowsCount())
{
}

void UiGridLayoutLuaProxy::SetHorizontalSpacing_FromGameThread(const uint32_t value)
{
    if (mHorizontalSpacing != value) {
        mHorizontalSpacing = value;
        mIsLuaDataDirty = true;
    }
}

void UiGridLayoutLuaProxy::SetVerticalSpacing_FromGameThread(const uint32_t value)
{
    if (mVerticalSpacing != value) {
        mVerticalSpacing = value;
        mIsLuaDataDirty = true;
    }
}

void UiGridLayoutLuaProxy::SetColumnsCount_FromGameThread(const uint32_t count)
{
    if (mColumnsCount != count) {
        mColumnsCount = count;
        mIsLuaDataDirty = true;
    }
}

void UiGridLayoutLuaProxy::SetRowsCount_FromGameThread(const uint32_t count)
{
    if (mRowsCount != count) {
        mRowsCount = count;
        mIsLuaDataDirty = true;
    }
}

void UiGridLayoutLuaProxy::SetAlignment_FromGameThread(
    const UiGridHorizontalAlignmentType horizontalAlignment, const UiGridVerticalAlignmentType verticalAlignment)
{
    if (mHorizontalAlignment != horizontalAlignment || mVerticalAlignment != verticalAlignment) {
        mHorizontalAlignment = horizontalAlignment;
        mVerticalAlignment = verticalAlignment;
        mIsLuaDataDirty = true;
    }
}

void UiGridLayoutLuaProxy::OnLuaThreadDataUpdated(const std::string& jsonParameters)
{
    static constexpr auto functionId = Hash64_CT("UiGridLayoutLuaProxy::OnLuaThreadDataUpdated");
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
                assert(replicator);
                const auto& gridLayout = std::static_pointer_cast<::EngineCore::GUI::UiGridLayout>(replicator);
                assert(gridLayout);
                gridLayout->SyncFromLuaJsonProperties(jsonStr);
            });
    }
}

std::string UiGridLayoutLuaProxy::GetGameThreadData()
{
    const auto& baseJsonStr = UiItemBaseLuaProxy::GetGameThreadData();
    auto jsonObj = nlohmann::json::parse(baseJsonStr);
    jsonObj["horizontal_spacing"] = mHorizontalSpacing;
    jsonObj["vertical_spacing"] = mVerticalSpacing;
    jsonObj["columns_count"] = mColumnsCount;
    jsonObj["rows_count"] = mRowsCount;
    jsonObj["horizontal_alignment"] = static_cast<int32_t>(mHorizontalAlignment);
    jsonObj["vertical_alignment"] = static_cast<int32_t>(mVerticalAlignment);
    return jsonObj.dump();
}
} // namespace Scripts
} // namespace EngineCore
