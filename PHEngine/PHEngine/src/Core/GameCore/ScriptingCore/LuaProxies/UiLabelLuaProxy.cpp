#include "UiLabelLuaProxy.h"

#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/GUI/UiElements/UiLabel.h"
#include "Core/GameCore/Scene.h"

#include <json/json.hpp>

using namespace EngineCore;
using namespace EngineCore::GUI;

namespace EngineCore {
namespace Scripts {
UiLabelLuaProxy::UiLabelLuaProxy(const std::shared_ptr<::EngineCore::GUI::UiLabel>& ownerLabel)
    : UiItemBaseLuaProxy(ownerLabel)
    , mText(ownerLabel->GetText())
    , mFontName(ownerLabel->GetFontName())
    , mOpacity(ownerLabel->GetOpacity())
    , mFontSize(ownerLabel->GetFontSize())
    , mTextLineWidth(ownerLabel->GetTextLineWidth())
    , mTextColor(ownerLabel->GetTextColor())
    , mTextHorizontalAlignment(ownerLabel->GetTextHorizontalAlignment())
{
}

void UiLabelLuaProxy::OnLuaThreadDataUpdated(const std::string& jsonParameters)
{
    static constexpr auto functionId = Hash64_CT("UiLabelLuaProxy::OnLuaThreadDataUpdated");
    if (const auto sceneSp = mSceneWp.lock()) {
        const auto replicatorId = GetReplicatorId();
        sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(
            eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
            mLuaProxyId,
            functionId,
            [sceneSp, replicatorId, jsonStr = jsonParameters]() {
                const auto& replicator = sceneSp->GetEngineToLuaReplicatorById(replicatorId);
                assert(replicator);
                const auto& uiLabel = std::static_pointer_cast<::EngineCore::GUI::UiLabel>(replicator);
                assert(uiLabel);
                uiLabel->SyncFromLuaJsonProperties(jsonStr);
            });
    }
}

std::string UiLabelLuaProxy::GetGameThreadData()
{
    const auto& baseJsonStr = UiItemBaseLuaProxy::GetGameThreadData();
    auto jsonObj = nlohmann::json::parse(baseJsonStr);
    const auto textColor = std::vector<float>({mTextColor.r, mTextColor.g, mTextColor.b});
    jsonObj["text"] = mText;
    jsonObj["text_opacity"] = mOpacity;
    jsonObj["font_size"] = mFontSize;
    jsonObj["text_line_width"] = mTextLineWidth;
    jsonObj["text_color"] = textColor;
    jsonObj["text_horizontal_alignment"] = mTextHorizontalAlignment;
    return jsonObj.dump();
}

void UiLabelLuaProxy::SetText_FromGameThread(const std::string& text)
{
    if (mText != text) {
        mText = text;
        mIsLuaDataDirty = true;
    }
}

void UiLabelLuaProxy::SetFontSize_FromGameThread(const float fontSize)
{
    if (!EngineMath::FloatsNearEqual(fontSize, mFontSize)) {
        mFontSize = fontSize;
        mIsLuaDataDirty = true;
    }
}

void UiLabelLuaProxy::SetTextLineWidth_FromGameThread(const float textLineWidth)
{
    if (!EngineMath::FloatsNearEqual(textLineWidth, mTextLineWidth)) {
        mTextLineWidth = textLineWidth;
        mIsLuaDataDirty = true;
    }
}

void UiLabelLuaProxy::SetTextColor_FromGameThread(const glm::vec3& texColor)
{
    if (!EngineMath::CheckSimilarityVec3(texColor, mTextColor)) {
        mTextColor = texColor;
        mIsLuaDataDirty = true;
    }
}

void UiLabelLuaProxy::SetTextHorizontalAlignment(const eTextHorizontalAlignmentType textHorizontalAlignment)
{
    if (textHorizontalAlignment != mTextHorizontalAlignment) {
        mTextHorizontalAlignment = textHorizontalAlignment;
        mIsLuaDataDirty = true;
    }
}

void UiLabelLuaProxy::SetOpacity_FromGameThread(const float opacity)
{
    if (!EngineMath::FloatsNearEqual(mOpacity, opacity)) {
        mOpacity = opacity;
        mIsLuaDataDirty = true;
    }
}
} // namespace Scripts
} // namespace EngineCore
