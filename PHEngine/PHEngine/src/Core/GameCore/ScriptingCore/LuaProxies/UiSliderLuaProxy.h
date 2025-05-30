#pragma once

#include "Core/GameCore/GUI/UiElements/UiSlider.h"
#include "UiItemBaseLuaProxy.h"

#include <glm/vec3.hpp>

using namespace EngineCore::GUI;

namespace EngineCore {
namespace Scripts {
class UiSliderLuaProxy : public UiItemBaseLuaProxy {
protected:
    float mMaxSliderValue{0.0f};
    float mMinSliderValue{0.0f};
    float mSliderValue{0.0f};
    float mSliderStep{0.0f};
    float mOpacity{1.0f};

    int32_t mSliderThicknessPixels{20};
    int32_t mBlobThicknessPixels{40};
    UiSlider::eUiSliderType mSliderType{UiSlider::eUiSliderType::Horizontal};

public:
    explicit UiSliderLuaProxy(const std::shared_ptr<UiSlider>& ownerUiItem);

    void EnableMouseInputReceiver();
    
    void OnLuaThreadDataUpdated(const std::string& jsonParameters) override;
    
    std::string GetGameThreadData() override;
    
    void SetMaxSliderValue_FromGameThread(const float maxSliderValue);

    void SetMinSliderValue_FromGameThread(const float minSliderValue);

    void SetSliderlValue_FromGameThread(const float sliderValue);

    void SetSliderStep_FromGameThread(const float sliderStep);

    void SetOpacity_FromGameThread(const float opacity);

    void SetSliderType_FromGameThread(const UiSlider::eUiSliderType sliderType);

    void SetSliderThicknessPixels_FromGameThread(const int32_t thicknessPixels);

    void SetSliderBlobThicknessPixels_FromGameThread(const int32_t thicknessPixels);
};
} // namespace Scripts
} // namespace EngineCore
