#include "UiScrollBar.h"

#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/UiScrollBarLuaProxy.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"
#include "Core/GraphicsCore/UiSceneProxy/UiScrollBarSceneProxy.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/UtilityCore/JsonUtilities.h"

#include <json/json.hpp>

using namespace Graphics::Proxy;
using namespace EngineCore::Scripts;

namespace EngineCore::GUI {
UiScrollBar::UiScrollBar(const std::string& name)
    : UiItemBase(name)
    , mMaxScrollValue(0.0f)
    , mMinScrollValue(0.0f)
    , mScrollValue(0.0f)
    , mScrollStep(0.0f)
{
}

UiScrollBar::~UiScrollBar()
{
}

void UiScrollBar::SetScrollValue(const float value)
{
    if (!EngineMath::FloatsNearEqual(value, mScrollValue)) {
        mScrollValue = value;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
    }
}

float UiScrollBar::GetScrollValue() const
{
    return mScrollValue;
}

void UiScrollBar::SetMaxScrollValue(const float maxScrollValue)
{
    if (!EngineMath::FloatsNearEqual(maxScrollValue, mMaxScrollValue)) {
        mMaxScrollValue = maxScrollValue;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

void UiScrollBar::SetMinScrollValue(const float minScrollValue)
{
    if (!EngineMath::FloatsNearEqual(minScrollValue, mMinScrollValue)) {
        mMinScrollValue = minScrollValue;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

void UiScrollBar::SetScrollStep(const float scrollStep)
{
    if (!EngineMath::FloatsNearEqual(scrollStep, mScrollStep)) {
        mScrollStep = scrollStep;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

float UiScrollBar::GetMaxScrollValue() const
{
    return mMaxScrollValue;
}

float UiScrollBar::GetMinScrollValue() const
{
    return mMinScrollValue;
}

float UiScrollBar::GetScrollStep() const
{
    return mScrollStep;
}

std::shared_ptr<::Graphics::Proxy::UiSceneProxyBase> UiScrollBar::CreateUiSceneProxy() const
{
    return std::make_shared<UiScrollBarSceneProxy>(this);
}

std::shared_ptr<LuaProxy> UiScrollBar::ReplicateLuaProxy()
{
    return std::make_shared<UiScrollBarLuaProxy>(std::static_pointer_cast<UiScrollBar>(shared_from_this()));
}

void UiScrollBar::OnPropertiesShouldBeUpdatedOnRenderThread()
{
    UiItemBase::OnPropertiesShouldBeUpdatedOnRenderThread();
    SyncDataOnRenderThread();
}

void UiScrollBar::OnPropertiesShouldBeUpdatedOnLuaThread()
{
    UiItemBase::OnPropertiesShouldBeUpdatedOnLuaThread();
    SyncDataOnLuaThread();
}

void UiScrollBar::SyncFromLuaJsonProperties(const std::string& luaJsonPropsStr)
{
    UiItemBase::SyncFromLuaJsonProperties(luaJsonPropsStr);

    const auto& jsonObj = nlohmann::json::parse(luaJsonPropsStr);
}

std::string UiScrollBar::GetUiTypeString() const
{
    return "UiScrollBar";
}

void UiScrollBar::OnRegistered()
{
    if (const auto& sceneSp = GetScene().lock()) {
        if (const auto& sceneRendererSp = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock()) {
            if (const auto& parentCanvasSp = mParentCanvas.lock()) {
                const auto thisSceneProxy = CreateUiSceneProxy();
                sceneRendererSp->RegisterUiSceneProxy_OnRenderThread(
                    std::static_pointer_cast<UiScrollBar>(shared_from_this()), thisSceneProxy, parentCanvasSp->GetUId());
            }
        }
    }
}

void UiScrollBar::OnUnregistered()
{
}

void UiScrollBar::SyncDataOnRenderThread()
{
    static constexpr uint64_t functionId = Hash64_CT("UiScrollBar::SyncDataOnRenderThread");
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
                            if (uiSceneProxy) {
                                // const auto& rectangleSceneProxy =
                                // std::static_pointer_cast<UiRectangleSceneProxy>(uiSceneProxy);
                                // rectangleSceneProxy->SetColor(color);
                                // rectangleSceneProxy->SetOpacity(opacity);
                                // rectangleSceneProxy->SetBorderRadius(borderRadius);
                            }
                        });
                }
            }
        }
    } else {
        mIsPropertiesShouldBeUpdatedOnRenderThread = true;
    }
}

void UiScrollBar::SyncDataOnLuaThread()
{
    static constexpr uint64_t functionId = Hash64_CT("UiScrollBar::SyncDataOnLuaThread");
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
                     scrollValue = mScrollValue,
                     maxScrollValue = mMaxScrollValue,
                     minScrollValue = mMinScrollValue,
                     scrollStep = mScrollStep]() {
                        if (const auto& scrollBarLuaProxy
                            = std::static_pointer_cast<UiScrollBarLuaProxy>(luaScriptProcessorSp->GetLuaProxy(luaProxyId))) {
                            scrollBarLuaProxy->SetScrollValue_FromGameThread(scrollValue);
                            scrollBarLuaProxy->SetMaxScrollValue_FromGameThread(maxScrollValue);
                            scrollBarLuaProxy->SetMinScrollValue_FromGameThread(minScrollValue);
                            scrollBarLuaProxy->SetScrollStep_FromGameThread(scrollStep);
                        }
                    });
            }
        }
    }
}
} // namespace EngineCore::GUI