#include "LevelEditorUiController.h"

#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"
#include "Core/GameCore/GUI/UiElements/UiHandler.h"
#include "Core/IoCore/DisplayDeviceDataProvider.h"
#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GameCore/Event/PauseGameEvent.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"
#include "Core/InterThreadCommunicationMgr.h"

using namespace IO;
using namespace EngineCore;
using namespace EngineCore::Scripts;
using namespace Thread;

namespace Game
{
    LevelEditorUiController::LevelEditorUiController(const std::weak_ptr<Scene> &scene)
        : mSceneWp(scene),
          mOverlayManager(std::make_shared<OverlayManager>(mSceneWp))
    {
    }

    void LevelEditorUiController::UnpausableTick(const float deltaTime)
    {
        mOverlayManager->UnpausableTick(deltaTime);
    }

    void LevelEditorUiController::Tick(const float deltaTime)
    {
        mOverlayManager->Tick(deltaTime);
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

    void LevelEditorUiController::CleanUp()
    {
        if (const auto &sceneSp = mSceneWp.lock())
        {
            if (const auto &luaScriptProcessorSp = sceneSp->GetInterThreadCommunicationManager().GetLuaScriptProcessor().lock())
            {
                const auto& luaScriptExecutor = std::dynamic_pointer_cast<LuaUiControllerExecutor>(luaScriptProcessorSp->GetLuaScriptExecutor(mExecutorId));
                assert(luaScriptExecutor);
                luaScriptExecutor->StopScript();
                luaScriptProcessorSp->UnregisterLuaScriptExecutor(mExecutorId);
            }
        }

        mOverlayManager->CleanUp();
    }

    void LevelEditorUiController::Initialize()
    {
        if (const auto &sceneSp = mSceneWp.lock())
        {
            if (const auto &luaScriptProcessorSp = sceneSp->GetInterThreadCommunicationManager().GetLuaScriptProcessor().lock())
            {
                static constexpr uint64_t functionId = Hash64_CT("LevelEditorUiController::Initialize");
                const auto& luaScriptExecutor = std::make_shared<LuaUiControllerExecutor>("Ui/Controllers/EditorUiController.lua");
                luaScriptExecutor->Initialize();
                mExecutorId = luaScriptExecutor->GetUId();
                sceneSp->GetInterThreadCommunicationManager().ExecuteOnLuaThread(eEnqueueJobPolicy::IF_DUPLICATE_NO_PUSH, 0, functionId, [luaScriptProcessorSp, luaScriptExecutor]()
                {
                    luaScriptProcessorSp->RegisterLuaScriptExecutor(luaScriptExecutor);
                    luaScriptExecutor->RunScript();
                });
            }
        }
    }
}