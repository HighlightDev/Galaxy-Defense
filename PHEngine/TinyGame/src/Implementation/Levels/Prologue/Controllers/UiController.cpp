#include "UiController.h"

#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GameCore/Components/InputComponent.h"
#include "Core/GameCore/Event/PauseGameThreadEvent.h"
#include "Core/GameCore/GUI/UiElements/UiHandler.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"
#include "Core/InterThreadCommunicationMgr.h"
#include "Core/IoCore/DisplayDeviceDataProvider.h"

using namespace IO;
using namespace EngineCore;
using namespace EngineCore::Scripts;
using namespace Thread;

namespace Game {
UiController::UiController(const std::weak_ptr<Scene>& scene)
    : mSceneWp(scene)
    , mOverlayManager(std::make_shared<OverlayManager>(mSceneWp))
    , mInputComponent(std::make_shared<InputComponent>(std::make_shared<ComponentData>("UiController Input Component")))
{
}

void UiController::UnpausableTick(const float deltaTime)
{
    mOverlayManager->UnpausableTick(deltaTime);
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
    Initialize();
    mOverlayManager->Initialize();
}

void UiController::CleanUp()
{
    if (const auto& sceneSp = mSceneWp.lock()) {
        if (const auto& luaScriptProcessorSp = sceneSp->GetInterThreadCommunicationManager().GetLuaScriptProcessor().lock()) {
            const auto& luaScriptExecutor
                = std::dynamic_pointer_cast<LuaUiControllerExecutor>(luaScriptProcessorSp->GetLuaScriptExecutor(mExecutorId));
            assert(luaScriptExecutor);
            luaScriptExecutor->StopScript();
            luaScriptProcessorSp->UnregisterLuaScriptExecutor(mExecutorId);
        }
    }

    mOverlayManager->CleanUp();
}

void UiController::Initialize()
{
    if (const auto& sceneSp = mSceneWp.lock()) {
        if (const auto& luaScriptProcessorSp = sceneSp->GetInterThreadCommunicationManager().GetLuaScriptProcessor().lock()) {
            static constexpr uint64_t functionId = Hash64_CT("UiController::Initialize");
            const auto& luaScriptExecutor = std::make_shared<LuaUiControllerExecutor>("Ui/Controllers/uiController.lua");
            mExecutorId = luaScriptExecutor->GetUId();
            sceneSp->GetInterThreadCommunicationManager().ExecuteOnLuaThread(
                eEnqueueJobPolicy::IF_DUPLICATE_NO_PUSH, 0, functionId, [luaScriptProcessorSp, luaScriptExecutor]() {
                    luaScriptProcessorSp->RegisterLuaScriptExecutor(luaScriptExecutor);
                    luaScriptExecutor->RunScript();
                });
        }
    }
}
} // namespace Game