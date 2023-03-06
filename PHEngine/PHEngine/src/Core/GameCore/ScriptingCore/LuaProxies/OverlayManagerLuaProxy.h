#pragma once

#include "LuaProxy.h"

#include <memory>
#include <string>

namespace EngineCore
{
    namespace GUI
    {
        class OverlayManager;
    }
}

namespace EngineCore
{
    namespace Scripts
    {
        class OverlayManagerLuaProxy
            : public LuaProxy
        {

            std::string mCurrentOverlayName;

        public:
            explicit OverlayManagerLuaProxy(const std::shared_ptr<::EngineCore::GUI::OverlayManager>& owner);

            void SetCurrentOverlay(const std::string& currentOverlayName);

            std::string GetCurrentOverlayName() const;

            void OpenOverlay(const std::string &overlayName);

            void CloseCurrentOverlay();
        };
    }
}
