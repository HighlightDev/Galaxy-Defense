#pragma once

#include "Core/GameCore/ITickable.h"

#include <string>

namespace EngineCore
{
    namespace GUI
    {
        class IUiOverlay : public ITickable
        {
        public:
            virtual std::string GetOverlayName() const = 0;

            virtual void OpenOverlay() = 0;

            virtual void CloseOverlay() = 0;

            virtual void Initialize() = 0;
        };
    }
}