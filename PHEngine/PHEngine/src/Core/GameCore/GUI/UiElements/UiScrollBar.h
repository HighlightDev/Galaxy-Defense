#pragma once

#include "UiItemBase.h"

namespace Graphics {
namespace Proxy {
class UiSceneProxyBase;
}
} // namespace Graphics

namespace EngineCore::GUI {
class UiScrollBar : public UiItemBase {

    float mMaxScrollValue{0.0f};
    float mMinScrollValue{0.0f};
    float mScrollValue{0.0f};
    float mScrollStep{0.0f};

public:
    explicit UiScrollBar(const std::string& name = std::string(""));

    ~UiScrollBar() override;

    void SetMaxScrollValue(const float maxScrollValue);

    void SetMinScrollValue(const float minScrollValue);

    void SetScrollStep(const float scrollStep);

    void SetScrollValue(const float value);

    float GetScrollValue() const;

    float GetMaxScrollValue() const;

    float GetMinScrollValue() const;

    float GetScrollStep() const;

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