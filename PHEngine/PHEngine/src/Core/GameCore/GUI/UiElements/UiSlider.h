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

    eUiSliderType mSliderType{eUiSliderType::Horizontal};

public:
    explicit UiSlider(const std::string& name = std::string(""));

    ~UiSlider() override;

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

    void SetSliderThicknessPixels(const int32_t thicknessPixels);

    eUiSliderType GetSliderType() const;

    void SetSliderType(const eUiSliderType sliderType);

    std::shared_ptr<::Graphics::Proxy::UiSceneProxyBase> CreateUiSceneProxy() const;

    std::shared_ptr<::EngineCore::Scripts::LuaProxy> ReplicateLuaProxy() override;

    void OnPropertiesShouldBeUpdatedOnRenderThread() override;

    void OnPropertiesShouldBeUpdatedOnLuaThread() override;

    void SyncFromLuaJsonProperties(const std::string& luaJsonPropsStr) override;

    std::string GetUiTypeString() const override;

protected:
    void OnRegistered() override;

    void OnUnregistered() override;

private:
    void SyncDataOnRenderThread();

    void SyncDataOnLuaThread();
};
} // namespace EngineCore::GUI