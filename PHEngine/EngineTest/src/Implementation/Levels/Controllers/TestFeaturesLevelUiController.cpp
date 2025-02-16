#include "TestFeaturesLevelUiController.h"

#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GameCore/Components/InputComponent.h"
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

namespace TestFeatures {
TestFeaturesLevelUiController::TestFeaturesLevelUiController(const std::weak_ptr<Scene>& scene)
    : mSceneWp(scene)
    , mOverlayManager(std::make_shared<OverlayManager>(mSceneWp))
    , mInputComponent(
          std::make_shared<InputComponent>(std::make_shared<ComponentData>("TestFeaturesLevelUiController Input Component")))
{
}

void TestFeaturesLevelUiController::UnpausableTick(const float deltaTime)
{
    mOverlayManager->UnpausableTick(deltaTime);
}

void TestFeaturesLevelUiController::Tick(const float deltaTime)
{
    mOverlayManager->Tick(deltaTime);
}

void TestFeaturesLevelUiController::OnPreLevelInit()
{
}

void TestFeaturesLevelUiController::OnLevelInit()
{
}

void TestFeaturesLevelUiController::OnPostLevelInit()
{
}

void TestFeaturesLevelUiController::PostPlayLevelFinished()
{
    Initialize();
    mOverlayManager->Initialize();
}

void TestFeaturesLevelUiController::CleanUp()
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

void TestFeaturesLevelUiController::Initialize()
{
    if (const auto& sceneSp = mSceneWp.lock()) {
        if (const auto& luaScriptProcessorSp = sceneSp->GetInterThreadCommunicationManager().GetLuaScriptProcessor().lock()) {
            static constexpr uint64_t functionId = Hash64_CT("TestFeaturesLevelUiController::Initialize");
            const auto& luaScriptExecutor = std::make_shared<LuaUiControllerExecutor>("Ui/Controllers/MainMenuUiController.lua");
            mExecutorId = luaScriptExecutor->GetUId();
            sceneSp->GetInterThreadCommunicationManager().ExecuteOnLuaThread(
                eEnqueueJobPolicy::IF_DUPLICATE_NO_PUSH, 0, functionId, [luaScriptProcessorSp, luaScriptExecutor]() {
                    luaScriptProcessorSp->RegisterLuaScriptExecutor(luaScriptExecutor);
                    luaScriptExecutor->RunScript();
                });
        }
    }
}
} // namespace TestFeatures