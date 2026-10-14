#pragma once

#include "Core/GameCore/ScriptingCore/EngineToLuaReplicatorBase.h"
#include "IUiOverlay.h"

using namespace EngineCore::Scripts;

namespace EngineCore {
class Scene;

namespace Scripts {
class LuaScriptProcessor;
class LuaProxy;
} // namespace Scripts
} // namespace EngineCore

namespace EngineCore {
namespace GUI {
class OverlayManager;
class UiCanvas;

class UiOverlay : public EngineToLuaReplicatorBase, public IUiOverlay {
    std::string mOverlayName;

    std::weak_ptr<::EngineCore::Scene> mSceneWp;

    std::shared_ptr<UiCanvas> mCanvas;

public:
    explicit UiOverlay(
        const std::string& overlayName,
        const std::weak_ptr<::EngineCore::Scene>& sceneWp,
        const std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor>& luaScriptProcessor);

    std::shared_ptr<::EngineCore::Scripts::LuaProxy> ReplicateLuaProxy() override;

    void SyncFromLuaJsonProperties(const std::string& luaJsonPropsStr) override;

    std::string GetOverlayName() const override;

    void OpenOverlay() override;

    void CloseOverlay() override;

    void Initialize() override;

    void SetOverlayCanvas(const std::shared_ptr<UiCanvas>& canvas);

    void Tick(const float deltaTimeSec, const float playSpeed) override;

    void UnpausableTick(const float deltaTimeSec, const float playSpeed) override;

    std::shared_ptr<::EngineCore::GUI::UiCanvas> GetCanvas() const override;

    void SubscribeOnAnimationFinished(const std::function<void(std::string)>& callback) override;

    bool HasFadeInAnimation() const override;

    bool HasFadeOutAnimation() const override;

    void CleanUp() override;
};
} // namespace GUI
} // namespace EngineCore