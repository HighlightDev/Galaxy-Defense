#include "UiTextBlock.h"

#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/UiTextBlockLuaProxy.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"
#include "Core/GraphicsCore/UiSceneProxy/UiTextBlockSceneProxy.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/UtilityCore/JsonUtilities.h"

#include <json/json.hpp>

using namespace EngineCore;
using namespace EngineCore::Scripts;
using namespace Graphics::Proxy;
using namespace Graphics::Renderer;
using namespace Resources;

namespace EngineCore {
namespace GUI {
UiTextBlock::UiTextBlock(const std::string& fontName, const std::string& name)
    : UiItemBase(name)
    , mFontName(fontName)
    , mTextLineWidthHeight(0)
{
    ext_assert(!mFontName.empty(), "Font name is empty.");
}

UiTextBlock::~UiTextBlock()
{
    LogInfo("UiTextBlock::dctor");
}

void UiTextBlock::OnRegistered()
{
    if (const auto& sceneSp = GetScene().lock()) {
        if (const auto& sceneRendererSp = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock()) {
            if (const auto& parentCanvasSp = mParentCanvas.lock()) {
                const auto thisSceneProxy = CreateUiSceneProxy();
                sceneRendererSp->RegisterUiSceneProxy_OnRenderThread(
                    std::static_pointer_cast<UiTextBlock>(shared_from_this()), thisSceneProxy, parentCanvasSp->GetUId());
            }
        }
    }
}

void UiTextBlock::OnUnregistered()
{
}

void UiTextBlock::OnPropertiesShouldBeUpdatedOnRenderThread()
{
    UiItemBase::OnPropertiesShouldBeUpdatedOnRenderThread();
    mTextLineWidthHeight = GetBoundingArea().GetHalfExtent() * 2;
    SyncDataOnRenderThread();
}

void UiTextBlock::OnPropertiesShouldBeUpdatedOnLuaThread()
{
    UiItemBase::OnPropertiesShouldBeUpdatedOnLuaThread();
    SyncDataOnLuaThread();
}

std::string UiTextBlock::GetUiTypeString() const
{
    return "UiTextBlock";
}

void UiTextBlock::SetText(const std::string& text)
{
    if (text != mText) {
        mText = text;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

std::string UiTextBlock::GetText() const
{
    return mText;
}

void UiTextBlock::SetOpacity(const float opacity)
{
    if (glm::abs(mOpacity - opacity) > EngineMath::ENGINE_FLOAT_EPSILON) {
        mOpacity = opacity;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

float UiTextBlock::GetOpacity() const
{
    return mOpacity;
}

std::string UiTextBlock::GetFontName() const
{
    return mFontName;
}

glm::ivec2 UiTextBlock::GetTextLineWidthHeight() const
{
    return mTextLineWidthHeight;
}

void UiTextBlock::SetFontSize(const int32_t fontSize)
{
    if (mFontSize != fontSize) {
        mFontSize = fontSize;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

int32_t UiTextBlock::GetFontSize() const
{
    return mFontSize;
}

void UiTextBlock::SetTextColor(const glm::vec3& color)
{
    if (!EngineMath::CheckSimilarityVec3(color, mTextColor)) {
        mTextColor = color;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

void UiTextBlock::SetTextColor(const uint32_t hexColor)
{
    SetTextColor(EngineMath::FromHexColorToVec3Color(hexColor));
}

glm::vec3 UiTextBlock::GetTextColor() const
{
    return mTextColor;
}

void UiTextBlock::SetTextHorizontalAlignment(const eTextHorizontalAlignmentType textHorizontalAlignment)
{
    if (mTextHorizontalAlignment != textHorizontalAlignment) {
        mTextHorizontalAlignment = textHorizontalAlignment;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

eTextHorizontalAlignmentType UiTextBlock::GetTextHorizontalAlignment() const
{
    return mTextHorizontalAlignment;
}

void UiTextBlock::SetTextVerticalAlignment(const eTextVerticalAlignmentType textVerticalAlignment)
{
    if (mTextVerticalAlignment != textVerticalAlignment) {
        mTextVerticalAlignment = textVerticalAlignment;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

eTextVerticalAlignmentType UiTextBlock::GetTextVerticalAlignment() const
{
    return mTextVerticalAlignment;
}

void UiTextBlock::SetRectangleColor(const glm::vec3& color)
{
    if (mRectangleColor != color) {
        mRectangleColor = color;
    }
}

void UiTextBlock::SetRectangleOpacity(const float opacity)
{
    if (!EngineMath::FloatsNearEqual(mRectangleOpacity, opacity)) {
        mRectangleOpacity = opacity;
    }
}

void UiTextBlock::SetRectangleRadius(const float borderRadius)
{
    if (!EngineMath::FloatsNearEqual(mRectangleRadius, borderRadius)) {
        mRectangleRadius = borderRadius;
    }
}

glm::vec3 UiTextBlock::GetRectangleColor() const
{
    return mRectangleColor;
}

float UiTextBlock::GetRectangleOpacity() const
{
    return mRectangleOpacity;
}

float UiTextBlock::GetRectangleRadius() const
{
    return mRectangleRadius;
}

glm::vec3 UiTextBlock::GetBorderColor() const
{
    return mBorderColor;
}

float UiTextBlock::GetBorderOpacity() const
{
    return mBorderOpacity;
}

float UiTextBlock::GetBorderRadius() const
{
    return mBorderRadius;
}

void UiTextBlock::SetBorderColor(const glm::vec3& color)
{
    if (mBorderColor != color) {
        mBorderColor = color;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

void UiTextBlock::SetBorderRadius(const float borderRadius)
{
    if (!EngineMath::FloatsNearEqual(mBorderRadius, borderRadius)) {
        mBorderRadius = borderRadius;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

void UiTextBlock::SetBorderOpacity(const float opacity)
{
    if (!EngineMath::FloatsNearEqual(mBorderOpacity, opacity)) {
        mBorderOpacity = opacity;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

std::shared_ptr<UiSceneProxyBase> UiTextBlock::CreateUiSceneProxy() const
{
    return std::make_shared<UiTextBlockSceneProxy>(this);
}

std::shared_ptr<LuaProxy> UiTextBlock::ReplicateLuaProxy()
{
    return std::make_shared<UiTextBlockLuaProxy>(std::static_pointer_cast<UiTextBlock>(shared_from_this()));
}

void UiTextBlock::SyncFromLuaJsonProperties(const std::string& luaJsonPropsStr)
{
    UiItemBase::SyncFromLuaJsonProperties(luaJsonPropsStr);

    bool bShouldUpdatePropertiesOnRT = false;

    const auto jsonObj = nlohmann::json::parse(luaJsonPropsStr);
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
        if (mFontSize != font_size) {
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
    if (jsonObj.contains("rectangle_opacity")) {
        const auto rectangle_opacity = jsonObj["rectangle_opacity"].get<float>();
        if (!EngineMath::FloatsNearEqual(mRectangleOpacity, rectangle_opacity)) {
            mRectangleOpacity = rectangle_opacity;
            bShouldUpdatePropertiesOnRT = true;
        }
    }
    if (jsonObj.contains("rectangle_color")) {
        const glm::vec3 rectangle_color = nlohmann_utilities::GetRgbFromJsonMap(jsonObj["rectangle_color"]);
        if (!EngineMath::CheckSimilarityVec3(rectangle_color, mRectangleColor)) {
            mRectangleColor = rectangle_color;
            bShouldUpdatePropertiesOnRT = true;
        }
    }
    if (jsonObj.contains("rectangle_radius")) {
        const auto rectangle_radius = jsonObj["rectangle_radius"].get<float>();
        if (!EngineMath::FloatsNearEqual(mRectangleRadius, rectangle_radius)) {
            mRectangleRadius = rectangle_radius;
            bShouldUpdatePropertiesOnRT = true;
        }
    }
    if (jsonObj.contains("border_color")) {
        const glm::vec3 border_color = nlohmann_utilities::GetRgbFromJsonMap(jsonObj["border_color"]);
        if (!EngineMath::CheckSimilarityVec3(border_color, mBorderColor)) {
            mBorderColor = border_color;
            bShouldUpdatePropertiesOnRT = true;
        }
    }
    if (jsonObj.contains("border_radius")) {
        const auto border_radius = jsonObj["border_radius"].get<float>();
        if (!EngineMath::FloatsNearEqual(mBorderRadius, border_radius)) {
            mBorderRadius = border_radius;
            bShouldUpdatePropertiesOnRT = true;
        }
    }
    if (jsonObj.contains("border_opacity")) {
        const auto border_opacity = jsonObj["border_opacity"].get<float>();
        if (!EngineMath::FloatsNearEqual(mBorderOpacity, border_opacity)) {
            mBorderOpacity = border_opacity;
            bShouldUpdatePropertiesOnRT = true;
        }
    }

    if (bShouldUpdatePropertiesOnRT) {
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
    }
}

void UiTextBlock::SyncDataOnRenderThread()
{
    static constexpr uint64_t functionId = Hash64_CT("UiTextBlock::SyncDataOnRenderThread");
    if (mIsSceneProxyReady.load(std::memory_order::seq_cst)) {
        if (const auto& sceneSp = GetScene().lock()) {
            if (const auto& canvasSp = GetParentCanvas().lock()) {
                if (const auto& sceneRenderer = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock()) {

                    sceneSp->GetInterThreadCommunicationManager().ExecuteOnRenderThread(
                        eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
                        GetUId(),
                        functionId,
                        [sceneRenderer,
                         myUId = GetUId(),
                         canvasUId = canvasSp->GetUId(),
                         opacity = mOpacity,
                         text = mText,
                         textLineWidthHeight = mTextLineWidthHeight,
                         fontSize = mFontSize,
                         textColor = mTextColor,
                         textHorizontalAlignment = mTextHorizontalAlignment,
                         textVerticalAlignment = mTextVerticalAlignment,
                         rectangleOpacity = mRectangleOpacity,
                         rectangleColor = mRectangleColor,
                         rectangleBorderRadius = mRectangleRadius,
                         borderColor = mBorderColor,
                         borderRadius = mBorderRadius,
                         borderOpacity = mBorderOpacity](
                            std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                            std::weak_ptr<EngineCore::Scene> sceneWp,
                            std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                            const auto& uiSceneProxy = sceneRenderer->GetUiSceneProxyByProxyId(myUId, canvasUId);
                            if (uiSceneProxy) {
                                const auto& textBlockSceneProxy = std::static_pointer_cast<UiTextBlockSceneProxy>(uiSceneProxy);
                                textBlockSceneProxy->SetOpacity(opacity);
                                textBlockSceneProxy->SetText(text);
                                textBlockSceneProxy->SetTextLineWidthHeight(textLineWidthHeight);
                                textBlockSceneProxy->SetFontSize(fontSize);
                                textBlockSceneProxy->SetTextColor(textColor);
                                textBlockSceneProxy->SetTextHorizontalAlignment(textHorizontalAlignment);
                                textBlockSceneProxy->SetTextVerticalAlignment(textVerticalAlignment);
                                textBlockSceneProxy->SetRectangleOpacity(rectangleOpacity);
                                textBlockSceneProxy->SetRectangleColor(rectangleColor);
                                textBlockSceneProxy->SetRectangleRadius(rectangleBorderRadius);
                                textBlockSceneProxy->SetBorderColor(borderColor);
                                textBlockSceneProxy->SetBorderRadius(borderRadius);
                                textBlockSceneProxy->SetBorderOpacity(borderOpacity);
                            }
                        });
                }
            }
        }
    } else {
        mIsPropertiesShouldBeUpdatedOnRenderThread = true;
    }
}

void UiTextBlock::SyncDataOnLuaThread()
{
    static constexpr uint64_t functionId = Hash64_CT("UiTextBlock::SyncDataOnLuaThread");
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
                     textVerticalAlignment = mTextVerticalAlignment,
                     rectangleColor = mRectangleColor,
                     rectangleOpacity = mRectangleOpacity,
                     rectangleBorderRadius = mRectangleRadius,
                     borderColor = mBorderColor,
                     borderRadius = mBorderRadius,
                     borderOpacity = mBorderOpacity](
                        std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                        std::weak_ptr<EngineCore::Scene> sceneWp,
                        std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                        if (const auto& textBlockLuaProxy
                            = std::static_pointer_cast<UiTextBlockLuaProxy>(luaScriptProcessorSp->GetLuaProxy(luaProxyId))) {
                            textBlockLuaProxy->SetOpacity_FromGameThread(opacity);
                            textBlockLuaProxy->SetText_FromGameThread(text);
                            textBlockLuaProxy->SetTextColor_FromGameThread(textColor);
                            textBlockLuaProxy->SetTextLineWidthHeight_FromGameThread(textLineWidthHeight);
                            textBlockLuaProxy->SetFontSize_FromGameThread(fontSize);
                            textBlockLuaProxy->SetTextHorizontalAlignment(textHorizontalAlignment);
                            textBlockLuaProxy->SetTextVerticalAlignment(textVerticalAlignment);
                            textBlockLuaProxy->SetRectangleColor_FromGameThread(rectangleColor);
                            textBlockLuaProxy->SetRectangleOpacity_FromGameThread(rectangleOpacity);
                            textBlockLuaProxy->SetRectangleBorderRadius_FromGameThread(rectangleBorderRadius);
                            textBlockLuaProxy->SetBorderColor_FromGameThread(borderColor);
                            textBlockLuaProxy->SetBorderRadius_FromGameThread(borderRadius);
                            textBlockLuaProxy->SetBorderOpacity_FromGameThread(borderOpacity);
                        }
                    });
            }
        }
    }
}
} // namespace GUI
} // namespace EngineCore