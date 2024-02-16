#pragma once

#include "UiCanvas.h"
#include "Core/GameCore/ITickable.h"

#include <vector>
#include <memory>

namespace EngineCore
{
    class Scene;

    namespace GUI
    {
        class UiHandler : public ITickable
        {
            std::weak_ptr<Scene> mOwner;

            std::vector<std::shared_ptr<UiCanvas>> mUiCanvases;

#ifdef DEBUG
            std::shared_ptr<UiCanvas> mDebugUiCanvas;
#endif

        public:
            UiHandler();

            void SetScene(const std::weak_ptr<::EngineCore::Scene> &owner);

            std::shared_ptr<UiCanvas> CreateCanvas(const ViewPortInfo &canvasScreenSize, const std::string& name = std::string(""));
#ifdef DEBUG
            std::shared_ptr<UiCanvas> CreateDebugCanvas(const ViewPortInfo &canvasScreenSize);
#endif

            void Tick(const float deltaTime) override;

            void UnpausableTick(const float deltaTime) override;

            std::shared_ptr<UiCanvas> GetCanvasByName(const std::string &canvasName) const;

            std::shared_ptr<UiCanvas> GetCanvasByUId(const uint32_t canvasId) const;

            void CleanUp();
        };
    }
}