#pragma once

#include "Core/GameCore/GUI/UiElements/UiSlider.h"
#include "Core/GameCore/ShaderImplementation/UiSliderShader.h"
#include "UiSceneProxyBase.h"

#include <glm/vec3.hpp>

using namespace EngineCore::ShaderImpl;
using namespace EngineCore::GUI;

namespace Graphics {
namespace Proxy {
class UiSliderSceneProxy : public UiSceneProxyBase {

    std::shared_ptr<UiSliderShader> mUiSliderShader;

    float mMaxSliderValue{0.0f};
    float mMinSliderValue{0.0f};
    float mSliderValue{0.0f};
    float mSliderStep{0.0f};
    float mOpacity{1.0f};

    int32_t mSliderThicknessPixels{20};
    glm::vec2 mSliderThicknessScale{1.0f, 1.0f};

    UiSlider::eUiSliderType mSliderType{UiSlider::eUiSliderType::Horizontal};

public:
    UiSliderSceneProxy(const UiSlider* uiSlider);

    ~UiSliderSceneProxy() override;

    void Render() override;

    void SetMaxSliderValue(const float maxSliderValue);

    void SetMinSliderValue(const float minSliderValue);

    void SetSliderValue(const float sliderValue);

    void SetSliderStep(const float sliderStep);

    void SetOpacity(const float opacity);

    void SetSliderThicknessPixels(const int32_t thicknessPixels);

    void SetSliderType(const UiSlider::eUiSliderType sliderType);

    void CleanUp() override;

    void OnSceneProxyRegistered() override;
};
} // namespace Proxy
} // namespace Graphics