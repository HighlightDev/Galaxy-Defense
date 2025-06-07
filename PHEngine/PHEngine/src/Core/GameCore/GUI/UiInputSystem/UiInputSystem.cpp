#include "UiInputSystem.h"

#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GameCore/Components/UiInputComponent.h"
#include "Core/GameCore/DataProviders/GeneralSystemSettingsDataProvider.h"
#include "Core/GameCore/GUI/UiElements/UiCanvas.h"
#include "Core/GameCore/LoggerExtension.h"

using namespace EngineCore::DataProviders;

namespace EngineCore {
namespace GUI {
size_t UiInputSystem::s_id = 0;

UiInputSystem::UiInputSystem(const std::weak_ptr<UiCanvas>& owner)
    : mScreenHeight(0)
    , mOwnerWp(owner)
    , mInputComponent(std::make_unique<UiInputComponent>(std::make_shared<ComponentData>(std::to_string(s_id++))))
    , mMouseKeyPressedPosition()
{
    mScreenHeight = static_cast<size_t>(GeneralSystemSettingsDataProvider::GetInstance()->GetWindowHeight());
    mMousePressedTimer.SetIntervalMs(300);
    mMousePressedTimer.SetIsRepeat(false);
    mMousePressedTimer.SetCallback(std::bind(&UiInputSystem::OnMousePressedTimerTimeout, this));
    mMousePressedTimer.SetIsPausable(false);
}

UiInputSystem::~UiInputSystem()
{
    WindowSizeChangedGameThreadEvent::GetInstance()->RemoveListener(WindowSizeChangedGameThreadEvent::GetInstanceId());
}

void UiInputSystem::Initialize()
{
    WindowSizeChangedGameThreadEvent::GetInstance()->AddListener(shared_from_this());
}

void UiInputSystem::Tick(const float deltaTime)
{
}

void UiInputSystem::UnpausableTick(const float deltaTime)
{
    const auto& mouseBindings = mInputComponent->GetMouseBindings();

    if (const auto& ownerSp = mOwnerWp.lock()) {
        if (ownerSp->IsVisible()) {
            const bool isMouseMoveEventDirty = mouseBindings->IsMouseMoveEventDirty();
            const auto& mouseMoveEventData = mouseBindings->GetLastMouseCursorPosition();
            mouseBindings->FlushMouseMoveEvent();
            const auto invertedScreenYPosition = mScreenHeight - mouseMoveEventData.y;
            mMouseKeyPressedPosition = glm::ivec2(mouseMoveEventData.x, invertedScreenYPosition);

            if (isMouseMoveEventDirty) {
                ownerSp->OnMousePositionChanged(mMouseKeyPressedPosition);
            }

            if (mouseBindings->GetKeyState(eMouseKeys::MouseButtonLeft) == KeyState::PRESSED) {
                if (!mIsMouseKeyPressed) {
                    mMousePressedTimer.StartTimer();
                }

                mIsMouseKeyPressed = true;
            } else if (mouseBindings->GetKeyState(eMouseKeys::MouseButtonLeft) == KeyState::RELEASED && mIsMouseKeyPressed) {
                if (mMousePressedTimer.IsRunning()) {
                    mMousePressedTimer.StopTimer();
                    ownerSp->OnMouseClicked(mMouseKeyPressedPosition);
                } else {
                    ownerSp->OnMouseReleased(mMouseKeyPressedPosition);
                }

                mIsMouseKeyPressed = false;
            }
        }
    }
}

void UiInputSystem::ProcessEvent(
    const WindowSizeChangedGameThreadEvent* sender, const WindowSizeChangedGameThreadEvent::EventData_t& data)
{
    mScreenHeight = static_cast<size_t>(std::get<0>(data).Height);
}

void UiInputSystem::OnMousePressedTimerTimeout()
{
    if (const auto& ownerSp = mOwnerWp.lock()) {
        if (ownerSp->IsVisible()) {
            ownerSp->OnMousePressed(mMouseKeyPressedPosition);
        }
    }
}
} // namespace GUI
} // namespace EngineCore