#pragma once

#include "IUiOverlay.h"
#include "Core/GameCore/ITickable.h"
#include "Core/GameCore/ScriptingCore/EngineToLuaReplicatorBase.h"

#include <memory>
#include <vector>

namespace EngineCore
{
    class Scene;

    namespace Scripts
    {
        class LuaProxy;
    }
}

using namespace EngineCore::Scripts;

namespace EngineCore
{
    namespace GUI
    {
        class OverlayManager : public EngineToLuaReplicatorBase,
                               public ITickable
        {
            std::vector<std::shared_ptr<IUiOverlay>> mOverlays;

            std::shared_ptr<IUiOverlay> mCurrentOpenedOverlay;

            std::weak_ptr<::EngineCore::Scene> mSceneWp;

        public:
            OverlayManager(const std::weak_ptr<Scene> &scene);

            std::shared_ptr<::EngineCore::Scripts::LuaProxy> ReplicateLuaProxy() override;

            void SyncFromLuaJsonProperties(const std::string& luaJsonPropsStr) override;

            void RegisterOverlay(std::shared_ptr<IUiOverlay> overlay);

            void UnregisterOverlay(std::shared_ptr<IUiOverlay> overlay);

            std::shared_ptr<IUiOverlay> GetOverlayByName(const std::string &name);

            void OpenOverlay(const std::string &overlayName);

            void CloseCurrentOverlay();

            bool CurrentOverlayOpened() const;

            std::string GetCurrentOpenedOverlayName() const;

            void Tick(const float deltaTime) override;

            void UnpausableTick(const float deltaTime) override;

            void Initialize();

            std::weak_ptr<::EngineCore::Scene> GetSceneWp() const;

        private:
            std::shared_ptr<IUiOverlay> FindOverlay(const std::string &overlayName) const;

            void SyncLuaThreadData();
        };
    }
}