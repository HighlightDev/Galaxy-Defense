#include "MainMenuLevelUiController.h"

#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GameCore/DataProviders/GeneralSystemSettingsDataProvider.h"
#include "Core/GameCore/Event/PauseGameEvent.h"
#include "Core/GameCore/GUI/UiElements/UiHandler.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"
#include "Core/InterThreadCommunicationMgr.h"

using namespace IO;
using namespace EngineCore;
using namespace EngineCore::DataProviders;
using namespace EngineCore::Scripts;
using namespace Thread;

namespace Game {
MainMenuLevelUiController::MainMenuLevelUiController(const std::weak_ptr<Scene>& scene)
    : mSceneWp(scene)
    , mOverlayManager(std::make_shared<OverlayManager>(mSceneWp))
{
}

void MainMenuLevelUiController::UnpausableTick(const float deltaTime)
{
    mOverlayManager->UnpausableTick(deltaTime);
}

void MainMenuLevelUiController::Tick(const float deltaTime)
{
    mOverlayManager->Tick(deltaTime);
}

void MainMenuLevelUiController::OnPreLevelInit()
{
}

void MainMenuLevelUiController::OnLevelInit()
{
}

void MainMenuLevelUiController::OnPostLevelInit()
{
    Initialize();
    mOverlayManager->Initialize();
}

void MainMenuLevelUiController::PostPlayLevelFinished()
{
}

void MainMenuLevelUiController::CleanUp()
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

void MainMenuLevelUiController::RestartLuaScripts()
{
    assert(!ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Lua"));

    if (const auto& sceneSp = mSceneWp.lock()) {
        if (const auto& luaScriptProcessorSp = sceneSp->GetInterThreadCommunicationManager().GetLuaScriptProcessor().lock()) {
            const auto& luaScriptExecutor
                = std::dynamic_pointer_cast<LuaUiControllerExecutor>(luaScriptProcessorSp->GetLuaScriptExecutor(mExecutorId));
            assert(luaScriptExecutor);
            luaScriptExecutor->SetIsEnabled(false);
            mOverlayManager->CleanUp();
            static constexpr uint64_t functionId = Hash64_CT("MainMenuLevelUiController::Initialize");
            sceneSp->GetInterThreadCommunicationManager().ExecuteOnLuaThread(
                eEnqueueJobPolicy::IF_DUPLICATE_NO_PUSH, 0, functionId, [luaScriptProcessorSp, luaScriptExecutor]() {
                    luaScriptExecutor->RestartScript();
                });
        }
    }
}

void MainMenuLevelUiController::Initialize()
{
    if (const auto& sceneSp = mSceneWp.lock()) {
        if (const auto& luaScriptProcessorSp = sceneSp->GetInterThreadCommunicationManager().GetLuaScriptProcessor().lock()) {
            static constexpr uint64_t functionId = Hash64_CT("MainMenuLevelUiController::Initialize");
            const auto& luaScriptExecutor = std::make_shared<LuaUiControllerExecutor>("MainMenuUiController.lua");
            luaScriptExecutor->Initialize();
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