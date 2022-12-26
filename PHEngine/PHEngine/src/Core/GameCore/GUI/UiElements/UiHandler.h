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

        public:
            UiHandler();

            void SetScene(const std::weak_ptr<::EngineCore::Scene> &owner);

            std::shared_ptr<UiCanvas> CreateCanvas(const ViewPortInfo &canvasScreenSize);

            virtual void Tick(const float deltaTime) override;

            virtual void UnpausableTick(const float deltaTime) override;
        };
    }
}