#pragma once

#include "Core/GameCore/ITickable.h"

#include <string>
#include <functional>
#include <memory>

namespace EngineCore
{
    namespace GUI
    {
        class UiCanvas;
        class IUiOverlay : public ITickable
        {
        public:
            virtual std::string GetOverlayName() const = 0;

            virtual void OpenOverlay() = 0;

            virtual void CloseOverlay() = 0;

            virtual void Initialize() = 0;
            
            virtual std::shared_ptr<::EngineCore::GUI::UiCanvas> GetCanvas() const = 0;

            virtual void SubscribeOnAnimationFinished(const std::function<void(std::string)>& callback) = 0;

            virtual bool HasFadeInAnimation() const = 0;

            virtual bool HasFadeOutAnimation() const = 0;
        };
    }
}