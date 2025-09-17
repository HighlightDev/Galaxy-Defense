#include "UiTextBlockLuaProxy.h"

#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/GUI/UiElements/UiTextBlock.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/UiItemBaseLuaProxy.h"

#include <json/json.hpp>

using namespace EngineCore;
using namespace EngineCore::GUI;

namespace EngineCore {
namespace Scripts {
UiTextBlockLuaProxy::UiTextBlockLuaProxy(const std::shared_ptr<::EngineCore::GUI::UiTextBlock>& ownerTextBlock)
    : UiItemBaseLuaProxy(ownerTextBlock)
    , mFontName(ownerTextBlock->GetFontName())
    , mFontSize(ownerTextBlock->GetFontSize())
    , mTextLineWidthHeight(ownerTextBlock->GetTextLineWidthHeight())
    , mTextColor(ownerTextBlock->GetTextColor())
    , mTextHorizontalAlignment(ownerTextBlock->GetTextHorizontalAlignment())
    , mTextVerticalAlignment(ownerTextBlock->GetTextVerticalAlignment())
    , mOpacity(ownerTextBlock->GetOpacity())
    , mRectangleColor(ownerTextBlock->GetRectangleColor())
    , mRectangleOpacity(ownerTextBlock->GetRectangleOpacity())
    , mRectangleRadius(ownerTextBlock->GetRectangleRadius())
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
            [sceneSp, replicatorId, jsonStr = jsonParameters](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
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
    const auto textColor = std::vector<float>({mTextColor.r, mTextColor.g, mTextColor.b});
    jsonObj["text"] = mText;
    jsonObj["text_opacity"] = mOpacity;
    jsonObj["font_size"] = mFontSize;
    jsonObj["text_color"] = textColor;
    jsonObj["text_horizontal_alignment"] = mTextHorizontalAlignment;
    jsonObj["text_vertical_alignment"] = mTextVerticalAlignment;
    jsonObj["rectangle_color"] = std::vector<float>({mRectangleColor.r, mRectangleColor.g, mRectangleColor.b});
    jsonObj["rectangle_opacity"] = mRectangleOpacity;
    jsonObj["rectangle_radius"] = mRectangleRadius;
    jsonObj["border_color"] = std::vector<float>({mBorderColor.r, mBorderColor.g, mBorderColor.b});
    jsonObj["border_radius"] = mBorderRadius;
    jsonObj["border_opacity"] = mBorderOpacity;
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

void UiTextBlockLuaProxy::SetTextLineWidthHeight_FromGameThread(const glm::ivec2& textLineWidthHeight)
{
    if (textLineWidthHeight != mTextLineWidthHeight) {
        mTextLineWidthHeight = textLineWidthHeight;
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

void UiTextBlockLuaProxy::SetTextVerticalAlignment(const eTextVerticalAlignmentType textVerticalAlignment)
{
    if (textVerticalAlignment != mTextVerticalAlignment) {
        mTextVerticalAlignment = textVerticalAlignment;
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

void UiTextBlockLuaProxy::SetRectangleColor_FromGameThread(const glm::vec3& color)
{
    if (!EngineMath::CheckSimilarityVec3(mRectangleColor, color)) {
        mRectangleColor = color;
        mIsLuaDataDirty = true;
    }
}

void UiTextBlockLuaProxy::SetRectangleOpacity_FromGameThread(const float opacity)
{
    if (!EngineMath::FloatsNearEqual(mRectangleOpacity, opacity)) {
        mRectangleOpacity = opacity;
        mIsLuaDataDirty = true;
    }
}

void UiTextBlockLuaProxy::SetRectangleBorderRadius_FromGameThread(const float borderRadius)
{
    if (!EngineMath::FloatsNearEqual(mRectangleRadius, borderRadius)) {
        mRectangleRadius = borderRadius;
        mIsLuaDataDirty = true;
    }
}
void UiTextBlockLuaProxy::SetBorderColor_FromGameThread(const glm::vec3& color)
{
    if (!EngineMath::CheckSimilarityVec3(mBorderColor, color)) {
        mBorderColor = color;
        mIsLuaDataDirty = true;
    }
}

void UiTextBlockLuaProxy::SetBorderRadius_FromGameThread(const float borderRadius)
{
    if (!EngineMath::FloatsNearEqual(mBorderRadius, borderRadius)) {
        mBorderRadius = borderRadius;
        mIsLuaDataDirty = true;
    }
}

void UiTextBlockLuaProxy::SetBorderOpacity_FromGameThread(const float opacity)
{
    if (!EngineMath::FloatsNearEqual(mBorderOpacity, opacity)) {
        mBorderOpacity = opacity;
        mIsLuaDataDirty = true;
    }
}
} // namespace Scripts
} // namespace EngineCore
