#include "MainMenuLevelUiController.h"

#include "Core/CommonCore/EngineConstants.h"
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

void MainMenuLevelUiController::UnpausableTick(const float deltaTimeSec)
{
    mOverlayManager->UnpausableTick(deltaTimeSec);
}

void MainMenuLevelUiController::Tick(const float deltaTimeSec)
{
    mOverlayManager->Tick(deltaTimeSec);
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
            ext_assert(luaScriptExecutor, "Failed to get Lua script executor in CleanUp");
            luaScriptExecutor->StopScript();
            luaScriptProcessorSp->UnregisterLuaScriptExecutor(mExecutorId);
        }
    }

    mOverlayManager->CleanUp();
}

void MainMenuLevelUiController::RestartLuaScripts()
{
    ext_assert(
        !ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName(EngineConstants::c_luaThreadName),
        "MainMenuLevelUiController::RestartLuaScripts called from Lua thread");

    if (const auto& sceneSp = mSceneWp.lock()) {
        if (const auto& luaScriptProcessorSp = sceneSp->GetInterThreadCommunicationManager().GetLuaScriptProcessor().lock()) {
            const auto& luaScriptExecutor
                = std::dynamic_pointer_cast<LuaUiControllerExecutor>(luaScriptProcessorSp->GetLuaScriptExecutor(mExecutorId));
            ext_assert(luaScriptExecutor, "Failed to get Lua script executor in RestartLuaScripts");
            luaScriptExecutor->SetIsEnabled(false);
            mOverlayManager->CleanUp();
            static constexpr uint64_t functionId = Hash64_CT("MainMenuLevelUiController::RestartLuaScripts");
            sceneSp->GetInterThreadCommunicationManager().ExecuteOnLuaThread(
                eEnqueueJobPolicy::IF_DUPLICATE_NO_PUSH,
                0,
                functionId,
                [luaScriptProcessorSp, luaScriptExecutor](
                    std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                    std::weak_ptr<EngineCore::Scene> sceneWp,
                    std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
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
                eEnqueueJobPolicy::IF_DUPLICATE_NO_PUSH,
                0,
                functionId,
                [luaScriptProcessorSp, luaScriptExecutor](
                    std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                    std::weak_ptr<EngineCore::Scene> sceneWp,
                    std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                    luaScriptProcessorSp->RegisterLuaScriptExecutor(luaScriptExecutor);
                    luaScriptExecutor->RunScript();
                });
        }
    }
}
} // namespace Game