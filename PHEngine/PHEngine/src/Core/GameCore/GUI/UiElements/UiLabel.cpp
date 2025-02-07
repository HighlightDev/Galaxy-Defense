#include "UiLabel.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/UiLabelLuaProxy.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"
#include "Core/GraphicsCore/UiSceneProxy/UiLabelSceneProxy.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/UtilityCore/JsonUtilities.h"

#include <json/json.hpp>

#include <functional>

using namespace EngineCore;
using namespace EngineCore::Scripts;
using namespace Graphics::Proxy;
using namespace Graphics::Renderer;

namespace EngineCore {
namespace GUI {
UiLabel::UiLabel(const std::string& fontName, const std::string& name)
    : UiItemBase(name)
    , mText("")
    , mOpacity(1.0f)
    , mFontName(fontName)
    , mTextLineWidth(1.0f)
    , mFontSize(5.0f)
    , mTextColor(glm::vec3())
{
    assert(mFontName.size());
}

UiLabel::~UiLabel()
{
}

void UiLabel::OnRegistered()
{
    if (const auto& sceneSp = GetScene().lock()) {
        if (const auto& sceneRendererSp = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock()) {
            if (const auto& parentCanvasSp = mParentCanvas.lock()) {
                const auto thisSceneProxy = CreateUiSceneProxy();
                sceneRendererSp->RegisterUiSceneProxy_OnRenderThread(
                    std::static_pointer_cast<UiLabel>(shared_from_this()), thisSceneProxy, parentCanvasSp->GetUId());
            }
        }
    }
}

void UiLabel::OnUnregistered()
{
}

void UiLabel::OnPropertiesShouldBeUpdatedOnRenderThread()
{
    UiItemBase::OnPropertiesShouldBeUpdatedOnRenderThread();
    mTextLineWidth = mNormalizedScale.x;
    SyncDataOnRenderThread();
}

void UiLabel::OnPropertiesShouldBeUpdatedOnLuaThread()
{
    UiItemBase::OnPropertiesShouldBeUpdatedOnLuaThread();

    SyncDataOnLuaThread();
}

void UiLabel::SyncFromLuaJsonProperties(const std::string& luaJsonPropsStr)
{
    UiItemBase::SyncFromLuaJsonProperties(luaJsonPropsStr);

    bool bShouldUpdatePropertiesOnRT = false;

    const auto& jsonObj = nlohmann::json::parse(luaJsonPropsStr);
    if (jsonObj.contains("text")) {
        const auto text = jsonObj["text"].get<std::string>();
        if (mText != text) {
            mText = text;
            bShouldUpdatePropertiesOnRT = true;
        }
    }
    if (jsonObj.contains("text_color")) {
        const glm::vec3 color = nlohmann_utilities::GetRgbFromJsonMap(jsonObj["text_color"]);
        if (!EngineMath::CheckSimilarityVec3(color, mTextColor)) {
            mTextColor = color;
            bShouldUpdatePropertiesOnRT = true;
        }
    }
    if (jsonObj.contains("opacity")) {
        const auto opacity = jsonObj["opacity"].get<float>();
        if (!EngineMath::FloatsNearEqual(mOpacity, opacity)) {
            mOpacity = opacity;
            bShouldUpdatePropertiesOnRT = true;
        }
    }
    if (jsonObj.contains("font_size")) {
        const auto font_size = jsonObj["font_size"].get<float>();
        if (!EngineMath::FloatsNearEqual(mFontSize, font_size)) {
            mFontSize = font_size;
            bShouldUpdatePropertiesOnRT = true;
        }
    }
    if (jsonObj.contains("text_line_width")) {
        const auto text_line_width = jsonObj["text_line_width"].get<float>();
        if (!EngineMath::FloatsNearEqual(mTextLineWidth, text_line_width)) {
            mTextLineWidth = text_line_width;
            bShouldUpdatePropertiesOnRT = true;
        }
    }
    if (jsonObj.contains("text_horizontal_alignment")) {
        const auto text_horizontal_alignment
            = static_cast<eTextHorizontalAlignmentType>(jsonObj["text_horizontal_alignment"].get<uint8_t>());
        if (text_horizontal_alignment != mTextHorizontalAlignment) {
            mTextHorizontalAlignment = text_horizontal_alignment;
            bShouldUpdatePropertiesOnRT = true;
        }
    }

    if (bShouldUpdatePropertiesOnRT) {
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
    }
}

void UiLabel::SetText(const std::string& text)
{
    if (text != mText) {
        mText = text;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

std::string UiLabel::GetText() const
{
    return mText;
}

void UiLabel::SetOpacity(const float opacity)
{
    if (glm::abs(mOpacity - opacity) > EngineMath::ENGINE_FLOAT_EPSILON) {
        mOpacity = opacity;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

float UiLabel::GetOpacity() const
{
    return mOpacity;
}

std::string UiLabel::GetFontName() const
{
    return mFontName;
}

float UiLabel::GetTextLineWidth() const
{
    return mTextLineWidth;
}

void UiLabel::SetFontSize(const float fontSize)
{
    if (glm::abs(mFontSize - fontSize) > EngineMath::ENGINE_FLOAT_EPSILON) {
        mFontSize = fontSize;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

float UiLabel::GetFontSize() const
{
    return mFontSize;
}

void UiLabel::SetTextColor(const glm::vec3& color)
{
    if (!EngineMath::CheckSimilarityVec3(color, mTextColor)) {
        mTextColor = color;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

void UiLabel::SetTextColor(const uint32_t hexColor)
{
    SetTextColor(EngineMath::FromHexColorToVec3Color(hexColor));
}

glm::vec3 UiLabel::GetTextColor() const
{
    return mTextColor;
}

void UiLabel::SetTextHorizontalAlignment(const eTextHorizontalAlignmentType textHorizontalAlignment)
{
    if (mTextHorizontalAlignment != textHorizontalAlignment) {
        mTextHorizontalAlignment = textHorizontalAlignment;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

eTextHorizontalAlignmentType UiLabel::GetTextHorizontalAlignment() const
{
    return mTextHorizontalAlignment;
}

std::shared_ptr<UiSceneProxyBase> UiLabel::CreateUiSceneProxy() const
{
    return std::make_shared<UiLabelSceneProxy>(this);
}

std::shared_ptr<LuaProxy> UiLabel::ReplicateLuaProxy()
{
    return std::make_shared<UiLabelLuaProxy>(std::static_pointer_cast<UiLabel>(shared_from_this()));
}

std::string UiLabel::GetUiTypeString() const
{
    return "UiLabel";
}

void UiLabel::SyncDataOnRenderThread()
{
    static constexpr uint64_t functionId = Hash64_CT("UiLabel::SyncDataOnRenderThread");
    if (mIsSceneProxyReady.load(std::memory_order::memory_order_seq_cst)) {
        if (const auto& sceneSp = GetScene().lock()) {
            if (const auto& canvasSp = GetParentCanvas().lock()) {
                if (const auto& sceneRenderer = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock()) {

                    sceneSp->GetInterThreadCommunicationManager().ExecuteOnRenderThread(
                        eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
                        GetUId(),
                        functionId,
                        [sceneRenderer,
                         myUid = GetUId(),
                         canvasUId = canvasSp->GetUId(),
                         opacity = mOpacity,
                         text = mText,
                         textLineWidth = mTextLineWidth,
                         fontSize = mFontSize,
                         textColor = mTextColor,
                         textHorizontalAlignment = mTextHorizontalAlignment]() {
                            const auto& uiSceneProxy = sceneRenderer->GetUiSceneProxyByProxyId(myUid, canvasUId);
                            if (uiSceneProxy) {
                                const auto& labelSceneProxy = std::static_pointer_cast<UiLabelSceneProxy>(uiSceneProxy);
                                labelSceneProxy->SetOpacity(opacity);
                                labelSceneProxy->SetText(text);
                                labelSceneProxy->SetTextLineWidth(textLineWidth);
                                labelSceneProxy->SetFontSize(fontSize);
                                labelSceneProxy->SetTextColor(textColor);
                                labelSceneProxy->SetTextHorizontalAlignment(textHorizontalAlignment);
                            }
                        });
                }
            }
        }
    } else {
        mIsPropertiesShouldBeUpdatedOnRenderThread = true;
    }
}

void UiLabel::SyncDataOnLuaThread()
{
    static constexpr uint64_t functionId = Hash64_CT("UiLabel::SyncDataOnLuaThread");
    if (mIsLuaProxyReady.load(std::memory_order::memory_order_seq_cst)) {
        if (const auto& sceneSp = GetScene().lock()) {
            if (const auto& luaScriptProcessorSp = GetLuaScriptProcessorWp().lock()) {
                SetIsPropertiesShouldBeUpdatedOnLuaThread(false);
                sceneSp->GetInterThreadCommunicationManager().ExecuteOnLuaThread(
                    eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
                    GetUId(),
                    functionId,
                    [luaScriptProcessorSp,
                     luaProxyId = GetLuaProxyId(),
                     opacity = mOpacity,
                     text = mText,
                     textColor = mTextColor,
                     textLineWidth = mTextLineWidth,
                     fontSize = mFontSize,
                     textHorizontalAlignment = mTextHorizontalAlignment]() {
                        if (const auto& labelLuaProxy
                            = std::static_pointer_cast<UiLabelLuaProxy>(luaScriptProcessorSp->GetLuaProxy(luaProxyId))) {
                            labelLuaProxy->SetOpacity_FromGameThread(opacity);
                            labelLuaProxy->SetText_FromGameThread(text);
                            labelLuaProxy->SetTextColor_FromGameThread(textColor);
                            labelLuaProxy->SetTextLineWidth_FromGameThread(textLineWidth);
                            labelLuaProxy->SetFontSize_FromGameThread(fontSize);
                            labelLuaProxy->SetTextHorizontalAlignment(textHorizontalAlignment);
                        }
                    });
            }
        }
    }
}
} // namespace GUI
} // namespace EngineCore