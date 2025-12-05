#include "CombatLevelUiController.h"

#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GameCore/DataProviders/GeneralSystemSettingsDataProvider.h"
#include "Core/GameCore/Event/PauseGameEvent.h"
#include "Core/GameCore/GUI/UiElements/UiHandler.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"
#include "Core/InterThreadCommunicationMgr.h"
#include "Implementation/LevelProgressSystem/LevelProgressController.h"
#include "Implementation/LuaExecutors/LuaCombatLevelUiControllerExecutor.h"

using namespace IO;
using namespace EngineCore;
using namespace EngineCore::DataProviders;
using namespace EngineCore::Scripts;
using namespace Thread;

namespace Game {
CombatLevelUiController::CombatLevelUiController(
    const std::weak_ptr<Scene>& scene, const std::shared_ptr<LevelProgressController>& lvlProgressController)
    : mSceneWp(scene)
    , mOverlayManager(std::make_shared<OverlayManager>(mSceneWp))
    , mLevelProgressController(lvlProgressController)
{
}

void CombatLevelUiController::UnpausableTick(const float deltaTimeSec)
{
    mOverlayManager->UnpausableTick(deltaTimeSec);
}

void CombatLevelUiController::Tick(const float deltaTimeSec)
{
    mOverlayManager->Tick(deltaTimeSec);
}

void CombatLevelUiController::OnPreLevelInit()
{
}

void CombatLevelUiController::OnLevelInit()
{
}

void CombatLevelUiController::OnPostLevelInit()
{
}

void CombatLevelUiController::PostPlayLevelFinished()
{
    Initialize();
    mOverlayManager->Initialize();
}

void CombatLevelUiController::CleanUp()
{
    if (const auto& sceneSp = mSceneWp.lock()) {
        if (const auto& luaScriptProcessorSp = sceneSp->GetInterThreadCommunicationManager().GetLuaScriptProcessor().lock()) {
            const auto& luaScriptExecutor = std::dynamic_pointer_cast<LuaCombatLevelUiControllerExecutor>(
                luaScriptProcessorSp->GetLuaScriptExecutor(mExecutorId));
            ext_assert(luaScriptExecutor, "CombatLevelUiController lua script executor is null in CleanUp");
            luaScriptExecutor->StopScript();
            luaScriptProcessorSp->UnregisterLuaScriptExecutor(mExecutorId);
        }
    }

    mOverlayManager->CleanUp();
}

void CombatLevelUiController::RestartLuaScripts()
{
    ext_assert(
        !ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Lua"),
        "CombatLevelUiController::RestartLuaScripts cannot be called from Lua thread");

    if (const auto& sceneSp = mSceneWp.lock()) {
        if (const auto& luaScriptProcessorSp = sceneSp->GetInterThreadCommunicationManager().GetLuaScriptProcessor().lock()) {
            const auto& luaScriptExecutor = std::dynamic_pointer_cast<LuaCombatLevelUiControllerExecutor>(
                luaScriptProcessorSp->GetLuaScriptExecutor(mExecutorId));
            ext_assert(luaScriptExecutor, "CombatLevelUiController lua script executor is null in RestartLuaScripts");
            luaScriptExecutor->SetIsEnabled(false);
            mOverlayManager->CleanUp();
            static constexpr uint64_t functionId = Hash64_CT("CombatLevelUiController::RestartLuaScripts");
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

void CombatLevelUiController::Initialize()
{
    if (const auto& sceneSp = mSceneWp.lock()) {
        if (const auto& luaScriptProcessorSp = sceneSp->GetInterThreadCommunicationManager().GetLuaScriptProcessor().lock()) {
            static constexpr uint64_t functionId = Hash64_CT("CombatLevelUiController::Initialize");
            const auto& luaScriptExecutor
                = std::make_shared<LuaCombatLevelUiControllerExecutor>("CombatUiController.lua", mLevelProgressController);
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