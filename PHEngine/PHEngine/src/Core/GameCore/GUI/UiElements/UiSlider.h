#pragma once

#include "UiItemBase.h"

namespace Graphics {
namespace Proxy {
class UiSceneProxyBase;
}
} // namespace Graphics

namespace EngineCore::GUI {
class UiSlider : public UiItemBase {

public:
    enum class eUiSliderType { Vertical, Horizontal };

private:
    float mMaxSliderValue{0.0f};
    float mMinSliderValue{0.0f};
    float mSliderValue{0.0f};
    float mSliderStep{0.0f};
    float mOpacity{1.0f};

    int32_t mSliderThicknessPixels{20};
    int32_t mBlobThicknessPixels{40};
    glm::vec2 mSliderThicknessScale{1.0f};
    glm::vec2 mBlobThicknessScale{0.0f};

    eUiSliderType mSliderType{eUiSliderType::Horizontal};

    glm::vec2 mSliderToCenterOffset{0.0f, 0.0f};
    glm::vec2 mBlobToCenterOffset{0.0f, 0.0f};

    glm::vec3 mSliderColor{1.0f, 1.0f, 1.0f};
    glm::vec3 mBlobColor{1.0f, 1.0f, 1.0f};

    glm::vec2 mAspectRatioScale{1.0f, 1.0f};

public:
    explicit UiSlider(const std::string& name = std::string(""));

    ~UiSlider() override;

    void UpdateAnchorTransform() override;

    void SetMaxSliderValue(const float maxSliderValue);

    void SetMinSliderValue(const float minSliderValue);

    void SetSliderStep(const float sliderStep);

    void SetSliderValue(const float value);

    float GetSliderValue() const;

    float GetMaxSliderValue() const;

    float GetMinSliderValue() const;

    float GetSliderStep() const;

    void SetOpacity(const float opacity);

    float GetOpacity() const;

    int32_t GetSliderThicknessPixels() const;

    int32_t GetBlobThicknessPixels() const;

    void SetSliderThicknessPixels(const int32_t thicknessPixels);

    void SetBlobThicknessPixels(const int32_t thicknessPixels);

    eUiSliderType GetSliderType() const;

    void SetSliderType(const eUiSliderType sliderType);

    glm::vec2 GetSliderToCenterOffset() const;

    glm::vec2 GetSliderThicknessScale() const;

    glm::vec2 GetBlobToCenterOffset() const;

    glm::vec2 GetBlobThicknessScale() const;

    glm::vec3 GetSliderColor() const;

    void SetSliderColor(const glm::vec3& color);

    glm::vec3 GetBlobColor() const;

    void SetBlobColor(const glm::vec3& color);

    glm::vec2 GetAspectRatioScale() const;

    void SetAspectRatioScale(const glm::vec2& aspectRatioScale);

    std::shared_ptr<::Graphics::Proxy::UiSceneProxyBase> CreateUiSceneProxy() const override;

    std::shared_ptr<::EngineCore::Scripts::LuaProxy> ReplicateLuaProxy() override;

    void OnPropertiesShouldBeUpdatedOnRenderThread() override;

    void OnPropertiesShouldBeUpdatedOnLuaThread() override;

    void SyncFromLuaJsonProperties(const std::string& luaJsonPropsStr) override;

    std::string GetUiTypeString() const override;

    float GetValueFromMousePosition(const glm::ivec2& mousePositionScreenSpace) const;

protected:
    void OnRegistered() override;

    void OnUnregistered() override;

private:
    void SyncDataOnRenderThread();

    void SyncDataOnLuaThread();

    void UpdateSliderToCenterOffset();

    void UpdateSliderThicknessScale();
};
} // namespace EngineCore::GUI