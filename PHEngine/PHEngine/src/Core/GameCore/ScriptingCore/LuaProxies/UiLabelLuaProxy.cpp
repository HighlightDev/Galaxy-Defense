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
    , mTextLineWidthHeight(ownerLabel->GetTextLineWidthHeight())
    , mTextColor(ownerLabel->GetTextColor())
    , mTextHorizontalAlignment(ownerLabel->GetTextHorizontalAlignment())
    , mTextVerticalAlignment(ownerLabel->GetTextVerticalAlignment())
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
    jsonObj["text_color"] = textColor;
    jsonObj["text_horizontal_alignment"] = mTextHorizontalAlignment;
    jsonObj["text_vertical_alignment"] = mTextVerticalAlignment;
    return jsonObj.dump();
}

void UiLabelLuaProxy::SetText_FromGameThread(const std::string& text)
{
    if (mText != text) {
        mText = text;
        mIsLuaDataDirty = true;
    }
}

void UiLabelLuaProxy::SetFontSize_FromGameThread(const int32_t fontSize)
{
    if (!fontSize != mFontSize) {
        mFontSize = fontSize;
        mIsLuaDataDirty = true;
    }
}

void UiLabelLuaProxy::SetTextLineWidthHeight_FromGameThread(const glm::ivec2& textLineWidthHeight)
{
    if (textLineWidthHeight != mTextLineWidthHeight) {
        mTextLineWidthHeight = textLineWidthHeight;
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

void UiLabelLuaProxy::SetTextVerticalAlignment(const eTextVerticalAlignmentType textVerticalAlignment)
{
    if (textVerticalAlignment != mTextVerticalAlignment) {
        mTextVerticalAlignment = textVerticalAlignment;
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
