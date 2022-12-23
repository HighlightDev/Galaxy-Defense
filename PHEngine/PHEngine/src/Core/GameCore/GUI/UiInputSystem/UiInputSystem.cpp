#include "UiInputSystem.h"

#include "Core/GameCore/GUI/UiElements/UiCanvas.h"
#include "Core/GameCore/Components/InputComponent.h"
#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/IoCore/DisplayDeviceDataProvider.h"
#include "Core/GameCore/LoggerExtension.h"

using namespace IO;

namespace EngineCore
{
    namespace GUI
    {
        size_t UiInputSystem::s_id = 0;

        UiInputSystem::UiInputSystem(const std::weak_ptr<UiCanvas> &owner)
            : mScreenHeight(0),
              mOwnerWp(owner),
              mInputComponent(std::make_unique<InputComponent>(ComponentData(std::to_string(s_id++)))),
              mMouseKeyPressedPosition()
        {
            mScreenHeight = static_cast<size_t>(DisplayDeviceDataProvider::GetInstance()->GetWindowHeight());
            mMousePressedTimer.SetIntervalMs(300);
            mMousePressedTimer.SetIsRepeat(false);
            mMousePressedTimer.SetCallback(std::bind(&UiInputSystem::OnMousePressedTimerTimeout, this));
        }

        void UiInputSystem::Tick(const float deltaTime)
        {
            auto &mouseBindings = mInputComponent->GetMouseBindings();

            if (const auto &ownerSp = mOwnerWp.lock())
            {
                if (ownerSp->IsVisible())
                {
                    const auto &mouseMoveEventData = mouseBindings.FlushMouseMoveEvent();
                    const auto invertedScreenYPosition = mScreenHeight - mouseMoveEventData.y;
                    mMouseKeyPressedPosition = glm::ivec2(mouseMoveEventData.x, invertedScreenYPosition);

                    if (mouseBindings.IsMouseMoveEventDirty())
                    {
                        ownerSp->OnMousePositionChanged(mMouseKeyPressedPosition);
                    }

                    if (mouseBindings.GetKeyState(eMouseKeys::MouseButtonLeft) == KeyState::PRESSED)
                    {
                        if (!mIsMouseKeyPressed)
                        {
                            mIsMouseKeyPressed = true;
                            mMousePressedTimer.StartTimer();
                        }
                    }
                    else if (mouseBindings.GetKeyState(eMouseKeys::MouseButtonLeft) == KeyState::RELEASED)
                    {
                        if (mMousePressedTimer.IsRunning() && mIsMouseKeyPressed)
                        {
                            mMousePressedTimer.StopTimer();
                            ownerSp->OnMouseClicked(mMouseKeyPressedPosition);
                            mIsMouseKeyPressed = false;
                        }
                        else if (mIsMouseKeyPressed)
                        {
                            ownerSp->OnMouseReleased(mMouseKeyPressedPosition);
                            mIsMouseKeyPressed = false;
                        }
                        
                    }
                }
            }
        }

        void UiInputSystem::OnMousePressedTimerTimeout()
        {
            if (const auto &ownerSp = mOwnerWp.lock())
            {
                if (ownerSp->IsVisible())
                {
                    ownerSp->OnMousePressed(mMouseKeyPressedPosition);
                }
            }
        }
    }
}