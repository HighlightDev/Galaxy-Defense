#pragma once

#include "Core/GameCore/ITickable.h"
#include "Core/GameCore/ScriptingCore/EngineToLuaReplicatorBase.h"
#include "IUiOverlay.h"

#include <memory>
#include <stack>
#include <unordered_set>
#include <vector>

namespace EngineCore {
class Scene;

namespace Scripts {
class LuaProxy;
}
} // namespace EngineCore

using namespace EngineCore::Scripts;

namespace EngineCore {
namespace GUI {
class OverlayManager : public EngineToLuaReplicatorBase, public ITickable {
    std::vector<std::shared_ptr<IUiOverlay>> mOverlays;

    std::vector<std::shared_ptr<IUiOverlay>> mBackgroundOverlays;

    std::unordered_set<std::string> mActiveBackgroundOverlays;

    std::stack<std::string> mActiveOverlaysHistory;

    std::shared_ptr<IUiOverlay> mCurrentOpenedOverlay;

    std::weak_ptr<::EngineCore::Scene> mSceneWp;

    bool mPendingAnimationFinishesToOpenOverlay{false};

public:
    OverlayManager(const std::weak_ptr<Scene>& scene);

    std::shared_ptr<::EngineCore::Scripts::LuaProxy> ReplicateLuaProxy() override;

    void SyncFromLuaJsonProperties(const std::string& luaJsonPropsStr) override;

    void RegisterOverlay(std::shared_ptr<IUiOverlay> overlay);

    void RegisterBackgroundOverlay(std::shared_ptr<IUiOverlay> overlay);

    void UnregisterOverlay(std::shared_ptr<IUiOverlay> overlay);

    void UnregisterBackgroundOverlay(std::shared_ptr<IUiOverlay> overlay);

    std::shared_ptr<IUiOverlay> GetOverlayByName(const std::string& name);

    std::shared_ptr<IUiOverlay> GetBackgroundOverlayByName(const std::string& name);

    void OpenOverlay(const std::string& overlayName, const bool isRestoreFromHistory = false);

    void OpenBackgroundOverlay(const std::string& overlayName);

    void CloseCurrentOverlay();

    void CloseOverlayAndClearHistory();

    void CloseBackgroundOverlay(const std::string& overlayName);

    bool CurrentOverlayOpened() const;

    std::string GetCurrentOpenedOverlayName() const;

    void Tick(const float deltaTime) override;

    void UnpausableTick(const float deltaTime) override;

    void Initialize();

    std::weak_ptr<::EngineCore::Scene> GetSceneWp() const;

    void CleanUp() override;

    bool GetPendingAnimationFinishesToOpenOverlay() const;

    void SetPendingAnimationFinishesToOpenOverlay(const bool value);

    std::shared_ptr<IUiOverlay> GetCurrentOpenedOverlay() const;

private:
    std::shared_ptr<IUiOverlay> FindOverlay(const std::string& overlayName) const;

    std::shared_ptr<IUiOverlay> FindBackgroundOverlay(const std::string& overlayName) const;

    void SyncLuaThreadData();
};
} // namespace GUI
} // namespace EngineCore