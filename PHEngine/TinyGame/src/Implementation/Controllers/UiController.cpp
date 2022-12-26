#include "UiController.h"

#include "Core/GameCore/Scene.h"
#include "Core/GameCore/GUI/UiElements/UiImage.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"
#include "Core/GameCore/GUI/UiElements/UiHandler.h"
#include "Core/IoCore/DisplayDeviceDataProvider.h"
#include "Core/GameCore/Components/InputComponent.h"
#include "Core/GameCore/Components/ComponentData/ComponentData.h"

using namespace IO;
using namespace EngineCore;

namespace Game
{
    UiController::UiController(const std::weak_ptr<Scene> &scene)
        : mSceneWp(scene),
          mPauseMenu(std::make_unique<PauseMenuUi>(scene)),
          mInputComponent(std::make_unique<InputComponent>(ComponentData("UiController Input Component"))),
          mPressButtonCooldown(0.0f)
    {
    }

    void UiController::UnpausableTick(const float deltaTime)
    {
        mPauseMenu->UnpausableTick(deltaTime);

        const auto &keyboardBindings = mInputComponent->GetKeyboardBindings();
        static constexpr float buttonCooldown = 0.5f;

        if (keyboardBindings.HasPressedKeys() &&
            KeyState::PRESSED == keyboardBindings.GetStateByKey(eKeyboardKeys::Escape))
        {
            if (mPressButtonCooldown >= buttonCooldown)
            {
                mPressButtonCooldown = 0.0f;
                if (mPauseMenu->IsVisible())
                {
                    mPauseMenu->HideMenu();
                }
                else
                {
                    mPauseMenu->ShowMenu();
                }
            }
        }

        mPressButtonCooldown += deltaTime;
    }

    void UiController::Tick(const float deltaTime)
    {
        mPauseMenu->Tick(deltaTime);
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
        mPauseMenu->Initialize();
    }
}