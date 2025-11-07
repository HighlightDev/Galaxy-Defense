#include "LevelEditorUiController.h"

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
LevelEditorUiController::LevelEditorUiController(const std::weak_ptr<Scene>& scene)
    : mSceneWp(scene)
    , mOverlayManager(std::make_shared<OverlayManager>(mSceneWp))
{
}

void LevelEditorUiController::UnpausableTick(const float deltaTimeSec)
{
    mOverlayManager->UnpausableTick(deltaTimeSec);
}

void LevelEditorUiController::Tick(const float deltaTimeSec)
{
    mOverlayManager->Tick(deltaTimeSec);
}

void LevelEditorUiController::OnPreLevelInit()
{
}

void LevelEditorUiController::OnLevelInit()
{
}

void LevelEditorUiController::OnPostLevelInit()
{
}

void LevelEditorUiController::PostPlayLevelFinished()
{
    Initialize();
    mOverlayManager->Initialize();
}

void LevelEditorUiController::RestartLuaScripts()
{
    if (const auto& sceneSp = mSceneWp.lock()) {
        sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(
            eEnqueueJobPolicy::IF_DUPLICATE_NO_PUSH,
            0,
            Hash64_CT("LevelEditorUiController::RestartLuaScripts"),
            [this, sceneSp](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                if (const auto& luaScriptProcessorSp
                    = sceneSp->GetInterThreadCommunicationManager().GetLuaScriptProcessor().lock()) {
                    const auto& luaScriptExecutor = std::dynamic_pointer_cast<LuaUiControllerExecutor>(
                        luaScriptProcessorSp->GetLuaScriptExecutor(mExecutorId));
                    assert(luaScriptExecutor);
                    luaScriptExecutor->SetIsEnabled(false);
                    mOverlayManager->CleanUp();
                    static constexpr uint64_t functionId = Hash64_CT("LevelEditorUiController::RestartLuaScripts");
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
            });
    }
}

void LevelEditorUiController::CleanUp()
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

void LevelEditorUiController::Initialize()
{
    if (const auto& sceneSp = mSceneWp.lock()) {
        if (const auto& luaScriptProcessorSp = sceneSp->GetInterThreadCommunicationManager().GetLuaScriptProcessor().lock()) {
            static constexpr uint64_t functionId = Hash64_CT("LevelEditorUiController::Initialize");
            const auto& luaScriptExecutor = std::make_shared<LuaUiControllerExecutor>("EditorUiController.lua");
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