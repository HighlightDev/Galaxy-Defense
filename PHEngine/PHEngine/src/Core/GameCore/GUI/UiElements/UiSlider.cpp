#include "UiSlider.h"

#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/UiSliderLuaProxy.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"
#include "Core/GraphicsCore/UiSceneProxy/UiSliderSceneProxy.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/UtilityCore/JsonUtilities.h"

#include <json/json.hpp>

using namespace Graphics::Proxy;
using namespace EngineCore::Scripts;

namespace EngineCore::GUI {
UiSlider::UiSlider(const std::string& name)
    : UiItemBase(name)
    , mMaxSliderValue(0.0f)
    , mMinSliderValue(0.0f)
    , mSliderValue(0.0f)
    , mSliderStep(0.0f)
{
}

UiSlider::~UiSlider()
{
}

void UiSlider::SetSliderValue(const float value)
{
    if (!EngineMath::FloatsNearEqual(value, mSliderValue)) {
        mSliderValue = value;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
    }
}

float UiSlider::GetSliderValue() const
{
    return mSliderValue;
}

void UiSlider::SetMaxSliderValue(const float maxSliderValue)
{
    if (!EngineMath::FloatsNearEqual(maxSliderValue, mMaxSliderValue)) {
        mMaxSliderValue = maxSliderValue;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

void UiSlider::SetMinSliderValue(const float minSliderValue)
{
    if (!EngineMath::FloatsNearEqual(minSliderValue, mMinSliderValue)) {
        mMinSliderValue = minSliderValue;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

void UiSlider::SetSliderStep(const float sliderStep)
{
    if (!EngineMath::FloatsNearEqual(sliderStep, mSliderStep)) {
        mSliderStep = sliderStep;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

float UiSlider::GetMaxSliderValue() const
{
    return mMaxSliderValue;
}

float UiSlider::GetMinSliderValue() const
{
    return mMinSliderValue;
}

float UiSlider::GetSliderStep() const
{
    return mSliderStep;
}

void UiSlider::SetOpacity(const float opacity)
{
    if (!EngineMath::FloatsNearEqual(opacity, mOpacity)) {
        mOpacity = opacity;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

float UiSlider::GetOpacity() const
{
    return mOpacity;
}

int32_t UiSlider::GetSliderThicknessPixels() const
{
    return mSliderThicknessPixels;
}

void UiSlider::SetSliderThicknessPixels(const int32_t thicknessPixels)
{
    if (thicknessPixels != mSliderThicknessPixels) {
        mSliderThicknessPixels = thicknessPixels;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

UiSlider::eUiSliderType UiSlider::GetSliderType() const
{
    return mSliderType;
}

void UiSlider::SetSliderType(const eUiSliderType sliderType)
{
    if (sliderType != mSliderType) {
        mSliderType = sliderType;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

std::shared_ptr<::Graphics::Proxy::UiSceneProxyBase> UiSlider::CreateUiSceneProxy() const
{
    return std::make_shared<UiSliderSceneProxy>(this);
}

std::shared_ptr<LuaProxy> UiSlider::ReplicateLuaProxy()
{
    return std::make_shared<UiSliderLuaProxy>(std::static_pointer_cast<UiSlider>(shared_from_this()));
}

void UiSlider::OnPropertiesShouldBeUpdatedOnRenderThread()
{
    UiItemBase::OnPropertiesShouldBeUpdatedOnRenderThread();
    SyncDataOnRenderThread();
}

void UiSlider::OnPropertiesShouldBeUpdatedOnLuaThread()
{
    UiItemBase::OnPropertiesShouldBeUpdatedOnLuaThread();
    SyncDataOnLuaThread();
}

void UiSlider::SyncFromLuaJsonProperties(const std::string& luaJsonPropsStr)
{
    UiItemBase::SyncFromLuaJsonProperties(luaJsonPropsStr);

    const auto& jsonObj = nlohmann::json::parse(luaJsonPropsStr);
    if (jsonObj.contains("max_slider_value")) {
        const auto max_slider_value = jsonObj["max_slider_value"].get<float>();
        if (!EngineMath::FloatsNearEqual(mMaxSliderValue, max_slider_value)) {
            mMaxSliderValue = max_slider_value;
            SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        }
    }
    if (jsonObj.contains("min_slider_value")) {
        const auto min_slider_value = jsonObj["min_slider_value"].get<float>();
        if (!EngineMath::FloatsNearEqual(mMinSliderValue, min_slider_value)) {
            mMinSliderValue = min_slider_value;
            SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        }
    }
    if (jsonObj.contains("slider_value")) {
        const auto slider_value = jsonObj["slider_value"].get<float>();
        if (!EngineMath::FloatsNearEqual(mSliderValue, slider_value)) {
            mSliderValue = slider_value;
            SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        }
    }
    if (jsonObj.contains("slider_step")) {
        const auto slider_step = jsonObj["slider_step"].get<float>();
        if (!EngineMath::FloatsNearEqual(mSliderStep, slider_step)) {
            mSliderStep = slider_step;
            SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        }
    }
    if (jsonObj.contains("slider_thickness_pixels")) {
        const auto slider_thickness_pixels = jsonObj["slider_thickness_pixels"].get<int32_t>();
        if (slider_thickness_pixels != mSliderThicknessPixels) {
            mSliderThicknessPixels = slider_thickness_pixels;
            SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        }
    }
    if (jsonObj.contains("slider_type")) {
        const auto slider_type = static_cast<eUiSliderType>(jsonObj["slider_type"].get<int32_t>());
        if (slider_type != mSliderType) {
            mSliderType = slider_type;
            SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        }
    }
    if (jsonObj.contains("opacity")) {
        const auto opacity = jsonObj["opacity"].get<float>();
        if (!EngineMath::FloatsNearEqual(mOpacity, opacity)) {
            mOpacity = opacity;
            SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        }
    }
}

std::string UiSlider::GetUiTypeString() const
{
    return "UiSlider";
}

void UiSlider::OnRegistered()
{
    if (const auto& sceneSp = GetScene().lock()) {
        if (const auto& sceneRendererSp = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock()) {
            if (const auto& parentCanvasSp = mParentCanvas.lock()) {
                const auto thisSceneProxy = CreateUiSceneProxy();
                sceneRendererSp->RegisterUiSceneProxy_OnRenderThread(
                    std::static_pointer_cast<UiSlider>(shared_from_this()), thisSceneProxy, parentCanvasSp->GetUId());
            }
        }
    }
}

void UiSlider::OnUnregistered()
{
}

void UiSlider::SyncDataOnRenderThread()
{
    static constexpr uint64_t functionId = Hash64_CT("UiSlider::SyncDataOnRenderThread");
    if (mIsSceneProxyReady.load(std::memory_order::memory_order_seq_cst)) {
        if (const auto& sceneSp = GetScene().lock()) {
            if (const auto& canvasSp = GetParentCanvas().lock()) {
                if (const auto& sceneRenderer = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock()) {

                    sceneSp->GetInterThreadCommunicationManager().ExecuteOnRenderThread(
                        eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
                        GetUId(),
                        functionId,
                        [this, sceneRenderer, myUId = GetUId(), canvasUId = canvasSp->GetUId()]() {
                            const auto& uiSceneProxy = sceneRenderer->GetUiSceneProxyByProxyId(myUId, canvasUId);
                            if (uiSceneProxy) {
                                const auto& sliderSceneProxy = std::static_pointer_cast<UiSliderSceneProxy>(uiSceneProxy);
                                sliderSceneProxy->SetOpacity(mOpacity);
                                sliderSceneProxy->SetSliderValue(mSliderValue);
                                sliderSceneProxy->SetMaxSliderValue(mMaxSliderValue);
                                sliderSceneProxy->SetMinSliderValue(mMinSliderValue);
                                sliderSceneProxy->SetSliderStep(mSliderStep);
                                sliderSceneProxy->SetSliderThicknessPixels(mSliderThicknessPixels);
                                sliderSceneProxy->SetSliderType(mSliderType);
                            }
                        });
                }
            }
        }
    } else {
        mIsPropertiesShouldBeUpdatedOnRenderThread = true;
    }
}

void UiSlider::SyncDataOnLuaThread()
{
    static constexpr uint64_t functionId = Hash64_CT("UiSlider::SyncDataOnLuaThread");
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
                     sliderValue = mSliderValue,
                     maxSliderValue = mMaxSliderValue,
                     minSliderValue = mMinSliderValue,
                     sliderStep = mSliderStep,
                     opacity = mOpacity,
                     sliderThicknessPixels = mSliderThicknessPixels,
                     sliderType = mSliderType]() {
                        if (const auto& sliderLuaProxy
                            = std::static_pointer_cast<UiSliderLuaProxy>(luaScriptProcessorSp->GetLuaProxy(luaProxyId))) {
                            sliderLuaProxy->SetSliderlValue_FromGameThread(sliderValue);
                            sliderLuaProxy->SetMaxSliderValue_FromGameThread(maxSliderValue);
                            sliderLuaProxy->SetMinSliderValue_FromGameThread(minSliderValue);
                            sliderLuaProxy->SetSliderStep_FromGameThread(sliderStep);
                            sliderLuaProxy->SetOpacity_FromGameThread(opacity);
                            sliderLuaProxy->SetSliderThicknessPixels_FromGameThread(sliderThicknessPixels);
                            sliderLuaProxy->SetSliderType_FromGameThread(sliderType);
                        }
                    });
            }
        }
    }
}
} // namespace EngineCore::GUI