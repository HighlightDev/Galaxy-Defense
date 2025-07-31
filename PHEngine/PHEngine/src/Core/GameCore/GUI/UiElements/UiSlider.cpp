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
        UpdateSliderToCenterOffset();
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
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
        UpdateSliderToCenterOffset();
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

void UiSlider::SetMinSliderValue(const float minSliderValue)
{
    if (!EngineMath::FloatsNearEqual(minSliderValue, mMinSliderValue)) {
        mMinSliderValue = minSliderValue;
        UpdateSliderToCenterOffset();
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

int32_t UiSlider::GetBlobThicknessPixels() const
{
    return mBlobThicknessPixels;
}

void UiSlider::SetSliderThicknessPixels(const int32_t thicknessPixels)
{
    if (thicknessPixels != mSliderThicknessPixels) {
        mSliderThicknessPixels = thicknessPixels;
        UpdateSliderToCenterOffset();
        UpdateSliderThicknessScale();
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

void UiSlider::SetBlobThicknessPixels(const int32_t thicknessPixels)
{
    if (thicknessPixels != mBlobThicknessPixels) {
        mBlobThicknessPixels = thicknessPixels;
        UpdateSliderToCenterOffset();
        UpdateSliderThicknessScale();
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

glm::vec2 UiSlider::GetAspectRatioScale() const
{
    return mAspectRatioScale;
}

void UiSlider::SetAspectRatioScale(const glm::vec2& aspectRatioScale)
{
    if (!EngineMath::CheckSimilarityVec2(mAspectRatioScale, aspectRatioScale)) {
        mAspectRatioScale = aspectRatioScale;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
    }
}

void UiSlider::UpdateSliderToCenterOffset()
{
    if (const auto& parentCanvasSp = mParentCanvas.lock()) {
        const auto canvasWidthPixels = parentCanvasSp->GetWidth();
        const auto canvasHeightPixels = parentCanvasSp->GetHeight();
        const float sliderDenominator = eUiSliderType::Horizontal == mSliderType ? static_cast<float>(canvasHeightPixels)
                                                                                 : static_cast<float>(canvasWidthPixels);
        if (sliderDenominator > 0.0f) {
            const float sliderThicknessHalf = (static_cast<float>(mSliderThicknessPixels) * 0.5f) / sliderDenominator;
            mSliderToCenterOffset = glm::vec2(
                mSliderType == eUiSliderType::Horizontal ? 0.0f : ((mWidth * 0.5f) / canvasWidthPixels) - sliderThicknessHalf,
                mSliderType == eUiSliderType::Horizontal ? ((mHeight * 0.5f) / canvasHeightPixels) - sliderThicknessHalf : 0.0f);

            const float blobThicknessHalf = (static_cast<float>(mBlobThicknessPixels) * 0.5f) / sliderDenominator;

            const float sliderNormalizedValue
                = glm::clamp((mSliderValue - mMinSliderValue) / (mMaxSliderValue - mMinSliderValue), 0.0f, 1.0f);
            const float blobOffset = sliderNormalizedValue
                * (eUiSliderType::Horizontal == mSliderType
                       ? (mWidth
                          - (mBlobThicknessPixels
                             * (static_cast<float>(canvasWidthPixels) / static_cast<float>(canvasHeightPixels))))
                       : (mHeight
                          - (mBlobThicknessPixels
                             * (static_cast<float>(canvasHeightPixels) / static_cast<float>(canvasWidthPixels)))))
                / (mSliderType == eUiSliderType::Horizontal ? static_cast<float>(canvasWidthPixels)
                                                            : static_cast<float>(canvasHeightPixels));
            mBlobToCenterOffset = glm::vec2(
                mSliderType == eUiSliderType::Horizontal ? blobOffset
                                                         : mSliderToCenterOffset.x + sliderThicknessHalf - blobThicknessHalf,
                mSliderType == eUiSliderType::Horizontal
                    ? mSliderToCenterOffset.y + sliderThicknessHalf - (mAspectRatioScale.y * blobThicknessHalf)
                    : blobOffset);
        }
    }
}

void UiSlider::UpdateSliderThicknessScale()
{
    if (const auto& parentCanvasSp = mParentCanvas.lock()) {
        const auto canvasWidthPixels = parentCanvasSp->GetWidth();
        const auto canvasHeightPixels = parentCanvasSp->GetHeight();
        const glm::vec2 normalizedScale(
            static_cast<float>(mWidth) / static_cast<float>(canvasWidthPixels),
            static_cast<float>(mHeight) / static_cast<float>(canvasHeightPixels));
        if (canvasWidthPixels > 0 && canvasHeightPixels > 0) {

            const float sliderThicknessSide = eUiSliderType::Horizontal == mSliderType ? canvasHeightPixels : canvasWidthPixels;
            if (mSliderThicknessPixels > sliderThicknessSide) {
                mSliderThicknessPixels = sliderThicknessSide;
            }
            const float sliderThicknessNormalized
                = static_cast<float>(mSliderThicknessPixels) / static_cast<float>(sliderThicknessSide);
            mSliderThicknessScale = glm::vec2(
                UiSlider::eUiSliderType::Horizontal == mSliderType ? normalizedScale.x : sliderThicknessNormalized,
                UiSlider::eUiSliderType::Vertical == mSliderType ? normalizedScale.y : sliderThicknessNormalized);

            const float blobThicknessNormalized
                = static_cast<float>(mBlobThicknessPixels) / static_cast<float>(sliderThicknessSide);
            mBlobThicknessScale = glm::vec2(blobThicknessNormalized);
        }
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
        UpdateSliderToCenterOffset();
        UpdateSliderThicknessScale();
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

void UiSlider::UpdateAnchorTransform()
{
    UiItemBase::UpdateAnchorTransform();
    if (const auto& parentCanvasSp = mParentCanvas.lock()) {
        SetAspectRatioScale(
            glm::vec2(1.0f, static_cast<float>(parentCanvasSp->GetWidth()) / static_cast<float>(parentCanvasSp->GetHeight())));
    }
    UpdateSliderToCenterOffset();
    UpdateSliderThicknessScale();
    SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
    SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
}

glm::vec2 UiSlider::GetSliderToCenterOffset() const
{
    return mSliderToCenterOffset;
}

glm::vec2 UiSlider::GetSliderThicknessScale() const
{
    return mSliderThicknessScale;
}

glm::vec2 UiSlider::GetBlobToCenterOffset() const
{
    return mBlobToCenterOffset;
}

glm::vec2 UiSlider::GetBlobThicknessScale() const
{
    return mBlobThicknessScale;
}

glm::vec3 UiSlider::GetSliderColor() const
{
    return mSliderColor;
}

void UiSlider::SetSliderColor(const glm::vec3& color)
{
    if (!EngineMath::CheckSimilarityVec3(mSliderColor, color)) {
        mSliderColor = color;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

glm::vec3 UiSlider::GetBlobColor() const
{
    return mBlobColor;
}

void UiSlider::SetBlobColor(const glm::vec3& color)
{
    if (!EngineMath::CheckSimilarityVec3(mBlobColor, color)) {
        mBlobColor = color;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

float UiSlider::GetValueFromMousePosition(const glm::ivec2& mousePositionScreenSpace) const
{
    float result = 0.0f;
    if (mWidth > 0 && mHeight > 0) {
        const auto& boundingArea = GetBoundingArea();
        float mouseExtensionNormalizedValue = eUiSliderType::Horizontal == mSliderType
            ? static_cast<float>(mousePositionScreenSpace.x - boundingArea.GetMin().x) / static_cast<float>(mWidth)
            : static_cast<float>(mousePositionScreenSpace.y - boundingArea.GetMin().y) / static_cast<float>(mHeight);
        mouseExtensionNormalizedValue = glm::clamp(mouseExtensionNormalizedValue, 0.0f, 1.0f);
        if (mSliderStep > 0.0f) {
            const float sliderRange = mMaxSliderValue - mMinSliderValue;
            const float stepCount = sliderRange / mSliderStep;
            const float stepSize = sliderRange / stepCount;
            const float stepIndex = std::round(mouseExtensionNormalizedValue * stepCount);
            mouseExtensionNormalizedValue = stepIndex * stepSize / sliderRange;
        }
        result = mMinSliderValue + (mouseExtensionNormalizedValue * (mMaxSliderValue - mMinSliderValue));
    }
    return result;
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
            UpdateSliderToCenterOffset();
            SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        }
    }
    if (jsonObj.contains("min_slider_value")) {
        const auto min_slider_value = jsonObj["min_slider_value"].get<float>();
        if (!EngineMath::FloatsNearEqual(mMinSliderValue, min_slider_value)) {
            mMinSliderValue = min_slider_value;
            UpdateSliderToCenterOffset();
            SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        }
    }
    if (jsonObj.contains("slider_value")) {
        const auto slider_value = jsonObj["slider_value"].get<float>();
        if (!EngineMath::FloatsNearEqual(mSliderValue, slider_value)) {
            mSliderValue = slider_value;
            UpdateSliderToCenterOffset();
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
            UpdateSliderToCenterOffset();
            UpdateSliderThicknessScale();
            SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        }
    }
    if (jsonObj.contains("blob_thickness_pixels")) {
        const auto blob_thickness_pixels = jsonObj["blob_thickness_pixels"].get<int32_t>();
        if (blob_thickness_pixels != mBlobThicknessPixels) {
            mBlobThicknessPixels = blob_thickness_pixels;
            UpdateSliderToCenterOffset();
            UpdateSliderThicknessScale();
            SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        }
    }
    if (jsonObj.contains("slider_type")) {
        const auto slider_type = static_cast<eUiSliderType>(jsonObj["slider_type"].get<int32_t>());
        if (slider_type != mSliderType) {
            mSliderType = slider_type;
            UpdateSliderToCenterOffset();
            UpdateSliderThicknessScale();
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
    if (jsonObj.contains("slider_color")) {
        const glm::vec3 color = nlohmann_utilities::GetRgbFromJsonMap(jsonObj["slider_color"]);
        if (!EngineMath::CheckSimilarityVec3(color, mSliderColor)) {
            mSliderColor = color;
            SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        }
    }
    if (jsonObj.contains("blob_color")) {
        const glm::vec3 color = nlohmann_utilities::GetRgbFromJsonMap(jsonObj["blob_color"]);
        if (!EngineMath::CheckSimilarityVec3(color, mBlobColor)) {
            mBlobColor = color;
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
                        [opacity = mOpacity,
                         sliderValue = mSliderValue,
                         maxSliderValue = mMaxSliderValue,
                         minSliderValue = mMinSliderValue,
                         sliderStep = mSliderStep,
                         sliderThicknessPixels = mSliderThicknessPixels,
                         blobThicknessPixels = mBlobThicknessPixels,
                         sliderType = mSliderType,
                         sliderToCenterOffset = mSliderToCenterOffset,
                         blobToCenterOffset = mBlobToCenterOffset,
                         sliderThicknessScale = mSliderThicknessScale,
                         blobThicknessScale = mBlobThicknessScale,
                         sliderColor = mSliderColor,
                         blobColor = mBlobColor,
                         aspectRatioScale = mAspectRatioScale,
                         sceneRenderer,
                         myUId = GetUId(),
                         canvasUId = canvasSp->GetUId()]() {
                            const auto& uiSceneProxy = sceneRenderer->GetUiSceneProxyByProxyId(myUId, canvasUId);
                            if (uiSceneProxy) {
                                const auto& sliderSceneProxy = std::static_pointer_cast<UiSliderSceneProxy>(uiSceneProxy);
                                sliderSceneProxy->SetOpacity(opacity);
                                sliderSceneProxy->SetSliderValue(sliderValue);
                                sliderSceneProxy->SetMaxSliderValue(maxSliderValue);
                                sliderSceneProxy->SetMinSliderValue(minSliderValue);
                                sliderSceneProxy->SetSliderStep(sliderStep);
                                sliderSceneProxy->SetSliderThicknessPixels(sliderThicknessPixels);
                                sliderSceneProxy->SetBlobThicknessPixels(blobThicknessPixels);
                                sliderSceneProxy->SetSliderType(sliderType);
                                sliderSceneProxy->SetSliderToCenterOffset(sliderToCenterOffset);
                                sliderSceneProxy->SetBlobToCenterOffset(blobToCenterOffset);
                                sliderSceneProxy->SetSliderThicknessScale(sliderThicknessScale);
                                sliderSceneProxy->SetBlobThicknessScale(blobThicknessScale);
                                sliderSceneProxy->SetSliderColor(sliderColor);
                                sliderSceneProxy->SetBlobColor(blobColor);
                                sliderSceneProxy->SetAspectRatioScale(aspectRatioScale);
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
                     blobThicknessPixels = mBlobThicknessPixels,
                     sliderType = mSliderType,
                     sliderColor = mSliderColor,
                     blobColor = mBlobColor]() {
                        if (const auto& sliderLuaProxy
                            = std::static_pointer_cast<UiSliderLuaProxy>(luaScriptProcessorSp->GetLuaProxy(luaProxyId))) {
                            sliderLuaProxy->SetSliderValue_FromGameThread(sliderValue);
                            sliderLuaProxy->SetMaxSliderValue_FromGameThread(maxSliderValue);
                            sliderLuaProxy->SetMinSliderValue_FromGameThread(minSliderValue);
                            sliderLuaProxy->SetSliderStep_FromGameThread(sliderStep);
                            sliderLuaProxy->SetOpacity_FromGameThread(opacity);
                            sliderLuaProxy->SetSliderThicknessPixels_FromGameThread(sliderThicknessPixels);
                            sliderLuaProxy->SetSliderType_FromGameThread(sliderType);
                            sliderLuaProxy->SetSliderBlobThicknessPixels_FromGameThread(blobThicknessPixels);
                            sliderLuaProxy->SetSliderColor_FromGameThread(sliderColor);
                            sliderLuaProxy->SetBlobColor_FromGameThread(blobColor);
                        }
                    });
            }
        }
    }
}
} // namespace EngineCore::GUI