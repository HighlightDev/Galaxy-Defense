#pragma once

#include "Core/GameCore/GUI/UiElements/UiItemBase.h"

namespace Graphics::Proxy {
class UiSceneProxyBase;
} // namespace Graphics::Proxy

namespace EngineCore::Scripts {
class LuaProxy;
} // namespace EngineCore::Scripts

namespace Game {

class UiSpaceshipUpgradeItem : public EngineCore::GUI::UiItemBase {
public:
    explicit UiSpaceshipUpgradeItem(const std::string& name = std::string(""));

    std::shared_ptr<::Graphics::Proxy::UiSceneProxyBase> CreateUiSceneProxy() const override;

    std::shared_ptr<::EngineCore::Scripts::LuaProxy> ReplicateLuaProxy() override;

    void OnPropertiesShouldBeUpdatedOnRenderThread() override;

    void OnPropertiesShouldBeUpdatedOnLuaThread() override;

    void SyncFromLuaJsonProperties(const std::string& luaJsonPropsStr) override;

    std::string GetUiTypeString() const override;

protected:
    void OnRegistered() override;

    void OnUnregistered() override;
};

} // namespace Game