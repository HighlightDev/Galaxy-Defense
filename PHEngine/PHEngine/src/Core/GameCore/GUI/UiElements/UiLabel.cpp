#include "UiLabel.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/UiProxies/UiLabelLuaProxy.h"
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
    , mTextLineWidthHeight()
    , mFontSize(15)
    , mTextColor(glm::vec3())
    , mOpacityProperty(std::make_shared<EngineObjectProperty<float>>(mOpacity, "Opacity", [this](const float newOpacityValue) {
        SetOpacity(newOpacityValue);
    }))
{
    ext_assert(mFontName.size(), "UiLabel::ctor: fontName is empty");
    ext_assert(!mProperties.count("Opacity"), "UiLabel::ctor: Property 'Opacity' already exists");
    mProperties.emplace("Opacity", mOpacityProperty);
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

bool UiLabel::OnPropertiesShouldBeUpdatedOnRenderThread()
{
    const bool parentUpdated = UiItemBase::OnPropertiesShouldBeUpdatedOnRenderThread();
    mTextLineWidthHeight = GetBoundingArea().GetHalfExtent() * 2;
    return parentUpdated && SyncDataOnRenderThread();
}

bool UiLabel::OnPropertiesShouldBeUpdatedOnLuaThread()
{
    return UiItemBase::OnPropertiesShouldBeUpdatedOnLuaThread() && SyncDataOnLuaThread();
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
    if (jsonObj.contains("text_opacity")) {
        const auto opacity = jsonObj["text_opacity"].get<float>();
        if (!EngineMath::FloatsNearEqual(mOpacity, opacity)) {
            mOpacity = opacity;
            bShouldUpdatePropertiesOnRT = true;
        }
    }
    if (jsonObj.contains("font_size")) {
        const auto font_size = jsonObj["font_size"].get<int32_t>();
        if (!mFontSize != font_size) {
            mFontSize = font_size;
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
    if (jsonObj.contains("text_vertical_alignment")) {
        const auto text_vertical_alignment
            = static_cast<eTextVerticalAlignmentType>(jsonObj["text_vertical_alignment"].get<uint8_t>());
        if (text_vertical_alignment != mTextVerticalAlignment) {
            mTextVerticalAlignment = text_vertical_alignment;
            bShouldUpdatePropertiesOnRT = true;
        }
    }
    if (jsonObj.contains("text_gradient_type")) {
        const auto text_gradient_type = static_cast<eTextGradientColorType>(jsonObj["text_gradient_type"].get<uint8_t>());
        if (text_gradient_type != mTextGradientColorType) {
            mTextGradientColorType = text_gradient_type;
            bShouldUpdatePropertiesOnRT = true;
        }
    }
    if (jsonObj.contains("gradient_color_start")) {
        const glm::vec3 gradient_color_start = nlohmann_utilities::GetRgbFromJsonMap(jsonObj["gradient_color_start"]);
        if (!EngineMath::CheckSimilarityVec3(gradient_color_start, mGradientTextColorStart)) {
            mGradientTextColorStart = gradient_color_start;
            bShouldUpdatePropertiesOnRT = true;
        }
    }
    if (jsonObj.contains("gradient_color_end")) {
        const glm::vec3 gradient_color_end = nlohmann_utilities::GetRgbFromJsonMap(jsonObj["gradient_color_end"]);
        if (!EngineMath::CheckSimilarityVec3(gradient_color_end, mGradientTextColorEnd)) {
            mGradientTextColorEnd = gradient_color_end;
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
    if (!EngineMath::FloatsNearEqual(opacity, mOpacity)) {
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

glm::ivec2 UiLabel::GetTextLineWidthHeight() const
{
    return mTextLineWidthHeight;
}

void UiLabel::SetFontSize(const int32_t fontSize)
{
    if (mFontSize != fontSize) {
        mFontSize = fontSize;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

int32_t UiLabel::GetFontSize() const
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

eTextVerticalAlignmentType UiLabel::GetTextVerticalAlignment() const
{
    return mTextVerticalAlignment;
}

eTextGradientColorType UiLabel::GetTextGradientColorType() const
{
    return mTextGradientColorType;
}

void UiLabel::SetTextGradientColorType(const eTextGradientColorType textGradientColorType)
{
    if (mTextGradientColorType != textGradientColorType) {
        mTextGradientColorType = textGradientColorType;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

glm::vec3 UiLabel::GetGradientTextColorStart() const
{
    return mGradientTextColorStart;
}

glm::vec3 UiLabel::GetGradientTextColorEnd() const
{
    return mGradientTextColorEnd;
}

void UiLabel::SetGradientTextColors(const glm::vec3& gradientTextColorStart, const glm::vec3& gradientTextColorEnd)
{
    if (!EngineMath::CheckSimilarityVec3(mGradientTextColorStart, gradientTextColorStart)
        || !EngineMath::CheckSimilarityVec3(mGradientTextColorEnd, gradientTextColorEnd)) {
        mGradientTextColorStart = gradientTextColorStart;
        mGradientTextColorEnd = gradientTextColorEnd;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

void UiLabel::SetTextVerticalAlignment(const eTextVerticalAlignmentType textVericalAlignment)
{
    if (mTextVerticalAlignment != textVericalAlignment) {
        mTextVerticalAlignment = textVericalAlignment;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

#ifdef DEBUG
void UiLabel::UpdateIsHiddenForDebugging(const bool isHiddenForDebugging)
{
    if (mIsHiddenForDebugging != isHiddenForDebugging) {
        mIsHiddenForDebugging = isHiddenForDebugging;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}
#endif

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

void UiLabel::SetTextNormalizedSize(const glm::vec2& size)
{
    mTextNormalizedSize = size;
}

void UiLabel::SetTextScreenSpaceSize(const glm::ivec2& size)
{
    mTextScreenSpaceSize = size;
}

glm::vec2 UiLabel::GetTextNormalizedSize() const
{
    return mTextNormalizedSize;
}

glm::ivec2 UiLabel::GetTextScreenSpaceSize() const
{
    return mTextScreenSpaceSize;
}

bool UiLabel::SyncDataOnRenderThread()
{
    static constexpr uint64_t functionId = Hash64_CT("UiLabel::SyncDataOnRenderThread");
    if (mIsSceneProxyReady.load(std::memory_order::seq_cst)) {
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
                         textLineWidthHeight = mTextLineWidthHeight,
                         fontSize = mFontSize,
                         textColor = mTextColor,
                         textHorizontalAlignment = mTextHorizontalAlignment,
                         textVerticalAlignment = mTextVerticalAlignment,
                         textGradientColorType = mTextGradientColorType,
                         gradientTextColorStart = mGradientTextColorStart,
                         gradientTextColorEnd = mGradientTextColorEnd](
                            std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                            std::weak_ptr<EngineCore::Scene> sceneWp,
                            std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                            const auto& uiSceneProxy = sceneRenderer->GetUiSceneProxyByProxyId(myUid, canvasUId);
                            if (uiSceneProxy) {
                                const auto& labelSceneProxy = std::static_pointer_cast<UiLabelSceneProxy>(uiSceneProxy);
                                labelSceneProxy->SetOpacity(opacity);
                                labelSceneProxy->SetText(text);
                                labelSceneProxy->SetTextLineWidthHeight(textLineWidthHeight);
                                labelSceneProxy->SetFontSize(fontSize);
                                labelSceneProxy->SetTextColor(textColor);
                                labelSceneProxy->SetTextHorizontalAlignment(textHorizontalAlignment);
                                labelSceneProxy->SetTextVerticalAlignment(textVerticalAlignment);
                                labelSceneProxy->SetGradientColor(
                                    textGradientColorType, gradientTextColorStart, gradientTextColorEnd);
                            }
                        });
                }
            }
        }

        return true;
    }
    return false;
}

bool UiLabel::SyncDataOnLuaThread()
{
    static constexpr uint64_t functionId = Hash64_CT("UiLabel::SyncDataOnLuaThread");
    if (mIsLuaProxyReady.load(std::memory_order::seq_cst)) {
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
                     textLineWidthHeight = mTextLineWidthHeight,
                     fontSize = mFontSize,
                     textHorizontalAlignment = mTextHorizontalAlignment,
                     textVerticalAlignment = mTextVerticalAlignment](
                        std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                        std::weak_ptr<EngineCore::Scene> sceneWp,
                        std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                        if (const auto& labelLuaProxy
                            = std::static_pointer_cast<UiLabelLuaProxy>(luaScriptProcessorSp->GetLuaProxy(luaProxyId))) {
                            labelLuaProxy->SetOpacity_FromGameThread(opacity);
                            labelLuaProxy->SetText_FromGameThread(text);
                            labelLuaProxy->SetTextColor_FromGameThread(textColor);
                            labelLuaProxy->SetTextLineWidthHeight_FromGameThread(textLineWidthHeight);
                            labelLuaProxy->SetFontSize_FromGameThread(fontSize);
                            labelLuaProxy->SetTextHorizontalAlignment(textHorizontalAlignment);
                            labelLuaProxy->SetTextVerticalAlignment(textVerticalAlignment);
                        }
                    });
            }
        }
        return true;
    }
    return false;
}
} // namespace GUI
} // namespace EngineCore