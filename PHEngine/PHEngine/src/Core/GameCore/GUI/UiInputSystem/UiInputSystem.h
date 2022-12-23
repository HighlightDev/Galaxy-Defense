#pragma once

#include "Core/GameCore/ITickable.h"
#include "Core/CommonCore/Timer.h"

#include <memory>
#include <glm/vec2.hpp>

using namespace EngineCore;

namespace EngineCore
{
    class InputComponent;

    namespace GUI
    {
        class UiCanvas;

        class UiInputSystem
            : public ITickable
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

            virtual void Tick(const float deltaTime) override;

        private:
            void OnMousePressedTimerTimeout();
        };
    }
}