#include "UiController.h"

#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"
#include "Core/GameCore/GUI/UiElements/UiHandler.h"
#include "Core/IoCore/DisplayDeviceDataProvider.h"
#include "Core/GameCore/Components/InputComponent.h"
#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Implementation/Ui/PauseMenuUi.h"
#include "Implementation/Ui/PauseSettingsMenuUi.h"
#include "Core/GameCore/Event/PauseGameThreadEvent.h"

using namespace IO;
using namespace EngineCore;

namespace Game
{
    UiController::UiController(const std::weak_ptr<Scene> &scene)
        : mSceneWp(scene),
          mOverlayManager(std::make_shared<OverlayManager>()),
          mInputComponent(std::make_unique<InputComponent>(ComponentData("UiController Input Component"))),
          mPressButtonCooldown(0.0f)
    {
        mOverlayManager->RegisterOverlay(std::make_shared<PauseMenuUi>("PauseMenu", scene, mOverlayManager));
        mOverlayManager->RegisterOverlay(std::make_shared<PauseSettingsMenuUi>("PauseSettingsMenu", scene, mOverlayManager));
    }

    void UiController::UnpausableTick(const float deltaTime)
    {
        mOverlayManager->UnpausableTick(deltaTime);

        const auto &keyboardBindings = mInputComponent->GetKeyboardBindings();
        static constexpr float buttonCooldown = 0.5f;

        if (keyboardBindings.HasPressedKeys())
        {
            if (KeyState::PRESSED == keyboardBindings.GetStateByKey(eKeyboardKeys::Escape))
            {
                if (mPressButtonCooldown >= buttonCooldown)
                {
                    mPressButtonCooldown = 0.0f;
                    if ("PauseMenu" == mOverlayManager->GetCurrentOpenedOverlayName())
                    {
                        mOverlayManager->CloseCurrentOverlay();
                        Event::PauseGameThreadEvent::GetInstance()->SendEvent(eExecutionOrder::POST_EXECUTION, false);
                    }
                    else
                    {
                        Event::PauseGameThreadEvent::GetInstance()->SendEvent(eExecutionOrder::POST_EXECUTION, true);
                        mOverlayManager->OpenOverlay("PauseMenu");
                    }
                }
            }
        }
        mPressButtonCooldown += deltaTime;
    }

    void UiController::Tick(const float deltaTime)
    {
        mOverlayManager->Tick(deltaTime);
    }

    void UiController::OnPreLevelInit()
    {
    }

    void UiController::OnLevelInit()
    {
    }

    void UiController::OnPostLevelInit()
    {
    }

    void UiController::PostPlayLevelFinished()
    {
        mOverlayManager->Initialize();
    }
}