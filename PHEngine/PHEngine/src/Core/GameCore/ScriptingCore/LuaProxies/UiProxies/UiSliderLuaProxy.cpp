#include "UiSliderLuaProxy.h"

#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/GUI/UiInputSystem/UiMouseInputReceiverSlider.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Scene.h"

#include <json/json.hpp>

using namespace EngineCore;

namespace EngineCore {
namespace Scripts {
UiSliderLuaProxy::UiSliderLuaProxy(const std::shared_ptr<UiSlider>& ownerSlider)
    : UiItemBaseLuaProxy(ownerSlider)
    , mMaxSliderValue(ownerSlider->GetMaxSliderValue())
    , mMinSliderValue(ownerSlider->GetMinSliderValue())
    , mSliderValue(ownerSlider->GetSliderValue())
    , mSliderStep(ownerSlider->GetSliderStep())
    , mOpacity(ownerSlider->GetOpacity())
    , mSliderThicknessPixels(ownerSlider->GetSliderThicknessPixels())
    , mBlobThicknessPixels(ownerSlider->GetBlobThicknessPixels())
    , mSliderType(ownerSlider->GetSliderType())
{
}

void UiSliderLuaProxy::OnLuaThreadDataUpdated(const std::string& jsonParameters)
{
    static constexpr auto functionId = Hash64_CT("UiSliderLuaProxy::OnLuaThreadDataUpdated");
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
                ext_assert(replicator, "UiSliderLuaProxy::OnLuaThreadDataUpdated: replicator is null");
                const auto& uiSlider = std::static_pointer_cast<UiSlider>(replicator);
                ext_assert(uiSlider, "UiSliderLuaProxy::OnLuaThreadDataUpdated: uiSlider is null");
                uiSlider->SyncFromLuaJsonProperties(jsonStr);
            });
    }
}

std::string UiSliderLuaProxy::GetGameThreadData()
{
    const auto& baseJsonStr = UiItemBaseLuaProxy::GetGameThreadData();
    auto jsonObj = nlohmann::json::parse(baseJsonStr);
    jsonObj["max_slider_value"] = mMaxSliderValue;
    jsonObj["min_slider_value"] = mMinSliderValue;
    jsonObj["slider_value"] = mSliderValue;
    jsonObj["slider_step"] = mSliderStep;
    jsonObj["opacity"] = mOpacity;
    jsonObj["slider_thickness_pixels"] = mSliderThicknessPixels;
    jsonObj["blob_thickness_pixels"] = mBlobThicknessPixels;
    jsonObj["slider_type"] = static_cast<int32_t>(mSliderType);
    return jsonObj.dump();
}

void UiSliderLuaProxy::EnableMouseInputReceiver()
{
    if (mIsMouseInputReceiverEnabled)
        return;

    static constexpr auto functionId = Hash64_CT("UiSliderLuaProxy::EnableMouseInputReceiver");
    if (const auto sceneSp = mSceneWp.lock()) {
        mIsMouseInputReceiverEnabled = true;
        const auto replicatorId = GetReplicatorId();
        sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(
            eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
            mLuaProxyId,
            functionId,
            [sceneSp, replicatorId](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                const auto& replicator = sceneSp->GetEngineToLuaReplicatorById(replicatorId);
                ext_assert(replicator, "UiSliderLuaProxy::EnableMouseInputReceiver: replicator is null");
                const auto& uiSlider = std::static_pointer_cast<::EngineCore::GUI::UiSlider>(replicator);
                ext_assert(uiSlider, "UiSliderLuaProxy::EnableMouseInputReceiver: uiSlider is null");
                uiSlider->SetMouseInputReceiver(std::make_shared<UiMouseInputReceiverSlider>(uiSlider));
            });
    }
}

void UiSliderLuaProxy::SetMaxSliderValue_FromGameThread(const float maxSliderValue)
{
    if (!EngineMath::FloatsNearEqual(mMaxSliderValue, maxSliderValue)) {

        mMaxSliderValue = maxSliderValue;
        mIsLuaDataDirty = true;
    }
}

void UiSliderLuaProxy::SetMinSliderValue_FromGameThread(const float minSliderValue)
{
    if (!EngineMath::FloatsNearEqual(mMinSliderValue, minSliderValue)) {
        mMinSliderValue = minSliderValue;
        mIsLuaDataDirty = true;
    }
}

void UiSliderLuaProxy::SetSliderValue_FromGameThread(const float sliderValue)
{
    if (!EngineMath::FloatsNearEqual(mSliderValue, sliderValue)) {
        mSliderValue = sliderValue;
        mIsLuaDataDirty = true;
    }
}

void UiSliderLuaProxy::SetSliderStep_FromGameThread(const float sliderStep)
{
    if (!EngineMath::FloatsNearEqual(mSliderStep, sliderStep)) {
        mSliderStep = sliderStep;
        mIsLuaDataDirty = true;
    }
}

void UiSliderLuaProxy::SetOpacity_FromGameThread(const float opacity)
{
    if (!EngineMath::FloatsNearEqual(mOpacity, opacity)) {
        mOpacity = opacity;
        mIsLuaDataDirty = true;
    }
}

void UiSliderLuaProxy::SetSliderType_FromGameThread(const UiSlider::eUiSliderType sliderType)
{
    if (mSliderType != sliderType) {
        mSliderType = sliderType;
        mIsLuaDataDirty = true;
    }
}

void UiSliderLuaProxy::SetSliderThicknessPixels_FromGameThread(const int32_t thicknessPixels)
{
    if (mSliderThicknessPixels != thicknessPixels) {
        mSliderThicknessPixels = thicknessPixels;
        mIsLuaDataDirty = true;
    }
}

void UiSliderLuaProxy::SetSliderBlobThicknessPixels_FromGameThread(const int32_t thicknessPixels)
{
    if (mBlobThicknessPixels != thicknessPixels) {
        mBlobThicknessPixels = thicknessPixels;
        mIsLuaDataDirty = true;
    }
}

void UiSliderLuaProxy::SetSliderColor_FromGameThread(const glm::vec3& color)
{
    if (!EngineMath::CheckSimilarityVec3(mSliderColor, color)) {
        mSliderColor = color;
        mIsLuaDataDirty = true;
    }
}

void UiSliderLuaProxy::SetBlobColor_FromGameThread(const glm::vec3& color)
{
    if (!EngineMath::CheckSimilarityVec3(mBlobColor, color)) {
        mBlobColor = color;
        mIsLuaDataDirty = true;
    }
}

} // namespace Scripts
} // namespace EngineCore
