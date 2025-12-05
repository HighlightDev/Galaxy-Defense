#include "UiRowLayoutLuaProxy.h"

#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/GUI/UiElements/UiRowLayout.h"
#include "Core/GameCore/Scene.h"
#include "Core/UtilityCore/EngineMath.h"

#include <json/json.hpp>

using namespace EngineCore;
using namespace EngineCore::GUI;

namespace EngineCore {
namespace Scripts {
UiRowLayoutLuaProxy::UiRowLayoutLuaProxy(const std::shared_ptr<::EngineCore::GUI::UiRowLayout>& ownerUiItem)
    : UiItemLuaProxy(ownerUiItem)
    , mSpacing(ownerUiItem->GetSpacing())
    , mAlignmentType(ownerUiItem->GetAlignment())
{
}

void UiRowLayoutLuaProxy::SetSpacing_FromGameThread(const uint32_t value)
{
    if (mSpacing != value) {
        mSpacing = value;
        mIsLuaDataDirty = true;
    }
}

void UiRowLayoutLuaProxy::SetAlignment_FromGameThread(const eUiRowAlignmentType alignmentType)
{
    if (mAlignmentType != alignmentType) {
        mAlignmentType = alignmentType;
        mIsLuaDataDirty = true;
    }
}

void UiRowLayoutLuaProxy::OnLuaThreadDataUpdated(const std::string& jsonParameters)
{
    static constexpr auto functionId = Hash64_CT("UiRowLayoutLuaProxy::OnLuaThreadDataUpdated");
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
                ext_assert(replicator, "UiRowLayoutLuaProxy::OnLuaThreadDataUpdated: replicator is null");
                const auto& rowLayout = std::static_pointer_cast<::EngineCore::GUI::UiRowLayout>(replicator);
                ext_assert(rowLayout, "UiRowLayoutLuaProxy::OnLuaThreadDataUpdated: rowLayout is null");
                rowLayout->SyncFromLuaJsonProperties(jsonStr);
            });
    }
}

std::string UiRowLayoutLuaProxy::GetGameThreadData()
{
    const auto& baseJsonStr = UiItemBaseLuaProxy::GetGameThreadData();
    auto jsonObj = nlohmann::json::parse(baseJsonStr);
    jsonObj["spacing"] = mSpacing;
    jsonObj["alignment"] = static_cast<int32_t>(mAlignmentType);
    return jsonObj.dump();
}
} // namespace Scripts
} // namespace EngineCore
