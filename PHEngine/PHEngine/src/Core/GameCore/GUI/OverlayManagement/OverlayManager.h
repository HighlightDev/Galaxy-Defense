#pragma once

#include "IUiOverlay.h"
#include "Core/GameCore/ITickable.h"

#include <memory>
#include <vector>

namespace EngineCore
{
    namespace GUI
    {
        class OverlayManager : public ITickable
        {
            std::vector<std::shared_ptr<IUiOverlay>> mOverlays;

            std::shared_ptr<IUiOverlay> mCurrentOpenedOverlay;

        public:
            OverlayManager();

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

        private:
            std::shared_ptr<IUiOverlay> FindOverlay(const std::string &overlayName) const;
        };
    }
}