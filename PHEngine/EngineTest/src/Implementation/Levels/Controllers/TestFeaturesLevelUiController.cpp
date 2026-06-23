#include "TestFeaturesLevelUiController.h"

#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GameCore/Components/InputComponent.h"
#include "Core/GameCore/DataProviders/GeneralSystemSettingsDataProvider.h"
#include "Core/GameCore/GUI/UiElements/UiHandler.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"
#include "Core/InterThreadCommunicationMgr.h"

using namespace IO;
using namespace EngineCore;
using namespace EngineCore::Scripts;
using namespace Thread;
using namespace EngineCore::DataProviders;

namespace TestFeatures {
TestFeaturesLevelUiController::TestFeaturesLevelUiController(const std::weak_ptr<Scene>& scene)
    : mSceneWp(scene)
    , mOverlayManager(std::make_shared<OverlayManager>(mSceneWp))
    , mInputComponent(
          std::make_shared<InputComponent>(std::make_shared<ComponentData>("TestFeaturesLevelUiController Input Component")))
{
}

void TestFeaturesLevelUiController::UnpausableTick(const float deltaTimeSec, const float playSpeed)
{
    mOverlayManager->UnpausableTick(deltaTimeSec, playSpeed);
}

void TestFeaturesLevelUiController::Tick(const float deltaTimeSec, const float playSpeed)
{
    mOverlayManager->Tick(deltaTimeSec, playSpeed);
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
            ext_assert(luaScriptExecutor, "Lua script executor not found in TestFeaturesLevelUiController::CleanUp");
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
            const auto luaScriptExecutor = std::make_shared<LuaUiControllerExecutor>("Ui/Controllers/MainMenuUiController.lua");
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
} // namespace TestFeatures