#include "UiSliderLuaProxy.h"

#include "Core/CommonCore/StringHash.h"
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
            [sceneSp, replicatorId, jsonStr = jsonParameters]() {
                const auto& replicator = sceneSp->GetEngineToLuaReplicatorById(replicatorId);
                assert(replicator);
                const auto& uiSlider = std::static_pointer_cast<UiSlider>(replicator);
                assert(uiSlider);
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
    jsonObj["slider_type"] = static_cast<int32_t>(mSliderType);
    return jsonObj.dump();
}

void UiSliderLuaProxy::SetMaxSliderValue_FromGameThread(const float maxSliderValue)
{
    mMaxSliderValue = maxSliderValue;
}

void UiSliderLuaProxy::SetMinSliderValue_FromGameThread(const float minSliderValue)
{
    mMinSliderValue = minSliderValue;
}

void UiSliderLuaProxy::SetSliderlValue_FromGameThread(const float sliderValue)
{
    mSliderValue = sliderValue;
}

void UiSliderLuaProxy::SetSliderStep_FromGameThread(const float sliderStep)
{
    mSliderStep = sliderStep;
}

void UiSliderLuaProxy::SetOpacity_FromGameThread(const float opacity)
{
    mOpacity = opacity;
}

void UiSliderLuaProxy::SetSliderType_FromGameThread(const UiSlider::eUiSliderType sliderType)
{
    mSliderType = sliderType;
}

void UiSliderLuaProxy::SetSliderThicknessPixels_FromGameThread(const int32_t thicknessPixels)
{
    mSliderThicknessPixels = thicknessPixels;
}

} // namespace Scripts
} // namespace EngineCore
