#pragma once

#include "Core/GameCore/ITickable.h"
#include "Core/CommonCore/Timer.h"
#include "Core/GameCore/Event/WindowSizeChangedEvent.h"

#include <memory>
#include <glm/vec2.hpp>

using namespace EngineCore;
using namespace Event;

namespace EngineCore
{
    class InputComponent;

    namespace GUI
    {
        class UiCanvas;

        class UiInputSystem
            : public ITickable
            , public WindowSizeChangedGameThreadEvent
            , public std::enable_shared_from_this<UiInputSystem>
        {
        private:
            static size_t s_id;

            size_t mScreenHeight;

            std::weak_ptr<UiCanvas> mOwnerWp;

            std::unique_ptr<::EngineCore::InputComponent> mInputComponent;

            GameThreadTimer mMousePressedTimer;

            glm::ivec2 mMouseKeyPressedPosition;

            bool mIsMouseKeyPressed{false};

        public:
            UiInputSystem(const std::weak_ptr<UiCanvas> &owner);

            ~UiInputSystem();

            void Initialize();

            void Tick(const float deltaTime) override;

            void UnpausableTick(const float deltaTime) override;

            void ProcessEvent(const WindowSizeChangedGameThreadEvent::EventData_t &data) override;

        private:
            void OnMousePressedTimerTimeout();
        };
    }
}