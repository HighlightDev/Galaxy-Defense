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
    : UiRectangle(name)
    , mFontName(fontName)
{
    assert(mFontName.size());
}

UiTextBlock::~UiTextBlock()
{
    volatile bool isDestroyed = true;
    LogInfo("UiTextBlock::~UiTextBlock() => destroyed: " + std::to_string(isDestroyed));
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
    UiRectangle::OnPropertiesShouldBeUpdatedOnRenderThread();
    mTextLineWidthHeight = GetBoundingArea().GetHalfExtent() * 2;
    SyncDataOnRenderThread();
}

void UiTextBlock::OnPropertiesShouldBeUpdatedOnLuaThread()
{
    UiRectangle::OnPropertiesShouldBeUpdatedOnLuaThread();

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
    UiRectangle::SyncFromLuaJsonProperties(luaJsonPropsStr);

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

    if (bShouldUpdatePropertiesOnRT) {
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
    }
}

void UiTextBlock::SyncDataOnRenderThread()
{
    static constexpr uint64_t functionId = Hash64_CT("UiTextBlock::SyncDataOnRenderThread");
    if (mIsSceneProxyReady.load(std::memory_order::memory_order_seq_cst)) {
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
                         textHorizontalAlignment = mTextHorizontalAlignment]() {
                            const auto& uiSceneProxy = sceneRenderer->GetUiSceneProxyByProxyId(myUId, canvasUId);
                            if (uiSceneProxy) {
                                const auto& textBlockSceneProxy = std::static_pointer_cast<UiTextBlockSceneProxy>(uiSceneProxy);
                                textBlockSceneProxy->SetOpacity(opacity);
                                textBlockSceneProxy->SetText(text);
                                textBlockSceneProxy->SetTextLineWidthHeight(textLineWidthHeight);
                                textBlockSceneProxy->SetFontSize(fontSize);
                                textBlockSceneProxy->SetTextColor(textColor);
                                textBlockSceneProxy->SetTextHorizontalAlignment(textHorizontalAlignment);
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
                     textLineWidthHeight = mTextLineWidthHeight,
                     fontSize = mFontSize,
                     textHorizontalAlignment = mTextHorizontalAlignment]() {
                        if (const auto& textBlockLuaProxy
                            = std::static_pointer_cast<UiTextBlockLuaProxy>(luaScriptProcessorSp->GetLuaProxy(luaProxyId))) {
                            textBlockLuaProxy->SetOpacity_FromGameThread(opacity);
                            textBlockLuaProxy->SetText_FromGameThread(text);
                            textBlockLuaProxy->SetTextColor_FromGameThread(textColor);
                            textBlockLuaProxy->SetTextLineWidthHeight_FromGameThread(textLineWidthHeight);
                            textBlockLuaProxy->SetFontSize_FromGameThread(fontSize);
                            textBlockLuaProxy->SetTextHorizontalAlignment(textHorizontalAlignment);
                        }
                    });
            }
        }
    }
}
} // namespace GUI
} // namespace EngineCore