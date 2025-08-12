#include "UiTextBlockLuaProxy.h"

#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/GUI/UiElements/UiTextBlock.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/UiRectangleLuaProxy.h"

#include <json/json.hpp>

using namespace EngineCore;
using namespace EngineCore::GUI;

namespace EngineCore {
namespace Scripts {
UiTextBlockLuaProxy::UiTextBlockLuaProxy(const std::shared_ptr<::EngineCore::GUI::UiTextBlock>& ownerTextBlock)
    : UiRectangleLuaProxy(ownerTextBlock)
{
}

void UiTextBlockLuaProxy::OnLuaThreadDataUpdated(const std::string& jsonParameters)
{
    static constexpr auto functionId = Hash64_CT("UiTextBlockLuaProxy::OnLuaThreadDataUpdated");
    if (const auto sceneSp = mSceneWp.lock()) {
        const auto replicatorId = GetReplicatorId();
        sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(
            eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
            mLuaProxyId,
            functionId,
            [sceneSp, replicatorId, jsonStr = jsonParameters]() {
                const auto& replicator = sceneSp->GetEngineToLuaReplicatorById(replicatorId);
                assert(replicator);
                const auto& uiTextBlock = std::static_pointer_cast<::EngineCore::GUI::UiTextBlock>(replicator);
                assert(uiTextBlock);
                uiTextBlock->SyncFromLuaJsonProperties(jsonStr);
            });
    }
}

std::string UiTextBlockLuaProxy::GetGameThreadData()
{
    const auto& baseJsonStr = UiItemBaseLuaProxy::GetGameThreadData();
    auto jsonObj = nlohmann::json::parse(baseJsonStr);
    return jsonObj.dump();
}

void UiTextBlockLuaProxy::SetText_FromGameThread(const std::string& text)
{
    if (mText != text) {
        mText = text;
        mIsLuaDataDirty = true;
    }
}

void UiTextBlockLuaProxy::SetFontSize_FromGameThread(const int32_t fontSize)
{
    if (fontSize != mFontSize) {
        mFontSize = fontSize;
        mIsLuaDataDirty = true;
    }
}

void UiTextBlockLuaProxy::SetTextLineWidth_FromGameThread(const float textLineWidth)
{
    if (!EngineMath::FloatsNearEqual(textLineWidth, mTextLineWidth)) {
        mTextLineWidth = textLineWidth;
        mIsLuaDataDirty = true;
    }
}

void UiTextBlockLuaProxy::SetTextColor_FromGameThread(const glm::vec3& texColor)
{
    if (!EngineMath::CheckSimilarityVec3(texColor, mTextColor)) {
        mTextColor = texColor;
        mIsLuaDataDirty = true;
    }
}

void UiTextBlockLuaProxy::SetTextHorizontalAlignment(const eTextHorizontalAlignmentType textHorizontalAlignment)
{
    if (textHorizontalAlignment != mTextHorizontalAlignment) {
        mTextHorizontalAlignment = textHorizontalAlignment;
        mIsLuaDataDirty = true;
    }
}

void UiTextBlockLuaProxy::SetOpacity_FromGameThread(const float opacity)
{
    if (!EngineMath::FloatsNearEqual(mOpacity, opacity)) {
        mOpacity = opacity;
        mIsLuaDataDirty = true;
    }
}
} // namespace Scripts
} // namespace EngineCore
