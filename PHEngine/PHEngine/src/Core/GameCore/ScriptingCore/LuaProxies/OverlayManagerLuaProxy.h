#pragma once

#include "LuaProxy.h"

#include <memory>
#include <string>
#include <unordered_set>

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

            std::unordered_set<std::string> mActiveBackgroundOverlays;

        public:
            explicit OverlayManagerLuaProxy(const std::shared_ptr<::EngineCore::GUI::OverlayManager> &owner);

            void SetCurrentOverlay(const std::string &currentOverlayName);

            void SetActiveBackgroundOverlays(const std::unordered_set<std::string> &backgroundOverlays);

            std::string GetCurrentOverlayName() const;

            void OpenOverlay(const std::string &overlayName);

            void OpenBackgroundOverlay(const std::string &overlayName);

            void OnLuaThreadDataUpdated(const std::string &jsonParameters) override;

            std::string GetGameThreadData() override;

            void CloseCurrentOverlay();

            void CloseBackgroundOverlay(const std::string& overlayName);
        };
    }
}
