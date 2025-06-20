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
UiTextBlock::UiTextBlock(const std::string& name)
    : UiRectangle(name)
{
}

UiTextBlock::~UiTextBlock()
{
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

float UiTextBlock::GetTextLineWidth() const
{
    return mTextLineWidth;
}

void UiTextBlock::SetFontSize(const float fontSize)
{
    if (glm::abs(mFontSize - fontSize) > EngineMath::ENGINE_FLOAT_EPSILON) {
        mFontSize = fontSize;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

float UiTextBlock::GetFontSize() const
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
                        [sceneRenderer, myUId = GetUId(), canvasUId = canvasSp->GetUId()]() {
                            const auto& uiSceneProxy = sceneRenderer->GetUiSceneProxyByProxyId(myUId, canvasUId);
                            if (uiSceneProxy) { }
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
                    [luaScriptProcessorSp, luaProxyId = GetLuaProxyId()]() {
                        if (const auto& textBlockLuaProxy
                            = std::static_pointer_cast<UiTextBlockLuaProxy>(luaScriptProcessorSp->GetLuaProxy(luaProxyId))) { }
                    });
            }
        }
    }
}
} // namespace GUI
} // namespace EngineCore