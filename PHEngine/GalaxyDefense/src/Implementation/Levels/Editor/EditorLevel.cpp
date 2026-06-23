#include "EditorLevel.h"

#include "Core/CommonCore/Random.h"
#include "Core/GameCore/BoundingBox3D.h"
#include "Core/GameCore/Components/ComponentCreators/BillboardComponentCreator.h"
#include "Core/GameCore/Components/ComponentData/BillboardComponentData.h"
#include "Core/GameCore/Components/LightComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/BillboardComponent.h"
#include "Core/GameCore/DataProviders/GeneralSystemSettingsDataProvider.h"
#include "Core/GameCore/Event/GameThreadEventDispatcher.h"
#include "Core/GameCore/Event/LuaThreadEventDispatcher.h"
#include "Core/GameCore/GUI/UiElements/Transform2D/BoundingBox2D.h"
#include "Core/GameCore/ScriptingCore/LuaScriptExecutors/LuaEngineScriptExecutor.h"
#include "Core/GraphicsCore/Material/MaterialParser.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialPropertySetter.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPerspectiveInfo.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"
#include "Core/UtilityCore/PlatformDependentFunctions.h"
#include "Implementation/DataProviders/LevelDataProvider.h"
#include "Implementation/Events/ChangeEditModeEvent.h"
#include "Implementation/Events/LevelAreaBBChangedEvent.h"
#include "Implementation/GalaxySceneCamera.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

using namespace IO;
using namespace EngineCore;
using namespace EngineCore::DataProviders;
using namespace EngineCore::Scripts;
using namespace EngineCore::GUI;
using namespace Graphics;

namespace Game {

EditorLevel::EditorLevel()
    : LevelBase("EditorLevel")
{
    Event::GameThreadEventDispatcher::GetInstance()
        ->RegisterEventsByType<Event::ChangeEditModeEvent, Event::LevelAreaBBChangedGameThreadEvent>();
    Event::LuaThreadEventDispatcher::GetInstance()->RegisterEventsByType<Event::LevelAreaBBChangedLuaThreadEvent>();
}

EditorLevel::~EditorLevel()
{
}

void EditorLevel::RunLuaBuildLevelScript()
{
    const auto sceneSp = mSceneWp.lock();
    ext_assert(sceneSp, "EditorLevel scene pointer is null in RunLuaBuildLevelScript");
    LuaEngineScriptExecutor mLuaLevelBuilder = LuaEngineScriptExecutor("EditorLvl.lua");
    mLuaLevelBuilder.SetScene(sceneSp);
    mLuaLevelBuilder.SetLuaScriptProcessor(sceneSp->GetInterThreadCommunicationManager().GetLuaScriptProcessor());
    mLuaLevelBuilder.RegisterCallbacks();
    mLuaLevelBuilder.RunScript();
    mLuaLevelBuilder.StopScript();
}

void EditorLevel::PreLevelInit()
{
    const auto sceneSp = mSceneWp.lock();
    ext_assert(sceneSp, "EditorLevel scene pointer is null in PreLevelInit");
    Base::PreLevelInit();
    mLevelEditorController = std::make_shared<LevelEditorController>(sceneSp);
    mLevelEditorController->OnPreLevelInit();

    mUiController = std::make_unique<LevelEditorUiController>(sceneSp);
    mUiController->OnPreLevelInit();

#ifdef DEBUG
    using namespace std::literals::chrono_literals;
    mFileWatcher
        = std::make_unique<FileWatcher>("./res/scripts/", 1000ms, [this](const std::string& path, const FileStatus fileStatus) {
              if (FileStatus::MODIFIED != fileStatus) {
                  return;
              }
              const auto beforeFileNameBeginIndex = EngineUtility::LastIndexOf(path, std::string(1, SLASH));
              if (beforeFileNameBeginIndex != std::string::npos) {
                  const auto& fileName = path.substr(beforeFileNameBeginIndex + 1);
                  const auto& fileExtension = fileName.substr(EngineUtility::IndexOf(fileName, ".") + 1);
                  if ("lua" == fileExtension) {
                      LogInfo("EditorLevel::FileWatcher::fileSatusChanged: fileName: ", fileName, " modified. Reload scripts.");
                      RestartLuaScripts();
                  }
              }
          });
#endif
}

void EditorLevel::CreateScene()
{
    const auto sceneSp = mSceneWp.lock();
    ext_assert(sceneSp, "EditorLevel scene pointer is null in CreateScene");

    constexpr float c_defaultLevelExtent = 60.0f;
    LevelDataProvider::GetInstance()->SetEditorLevelAreaBoundingBox(
        BoundingBox2D<glm::vec2>(glm::vec2(), glm::vec2(c_defaultLevelExtent)), false, true);

    const auto& a_sceneCenterActorDummy = sceneSp->GetActorByName("SceneCenterActorDummy");
    ext_assert(a_sceneCenterActorDummy, "EditorLevel scene center actor dummy not found");

    const auto& spaceCamera = std::make_shared<GalaxySceneCamera>(
        "LevelMainCamera",
        eCameraType::MAIN_THIRD_PERSON_CAMERA,
        sceneSp,
        ViewPortInfo(
            0,
            0,
            GeneralSystemSettingsDataProvider::GetInstance()->GetWindowWidth(),
            GeneralSystemSettingsDataProvider::GetInstance()->GetWindowHeight()),
        std::make_shared<ViewPerspectiveInfo>(glm::radians<float>(60.0f), 16.0f / 9.0f, 0.1f, 500.0f),
        38.88f,
        0.0f,
        200.0f);

    spaceCamera->SetMinDistanceFromTargetToCamera(20.0f);
    spaceCamera->InitializeMaxDistanceToCamera(
        BoundingBox3D(glm::vec3(), glm::vec3(c_defaultLevelExtent, 5.0f, c_defaultLevelExtent)), glm::radians(60.0f));
    spaceCamera->SetDistanceFromTargetToCamera(spaceCamera->GetMaxDistanceFromTargetToCamera());
    sceneSp->RegisterMainCamera(spaceCamera);
    spaceCamera->SetThirdPersonTarget(a_sceneCenterActorDummy);
    spaceCamera->SetTimeForInterpolation(6e-1f);

    const auto& a_skybox = sceneSp->GetActorByName("SkyboxActor");
    ext_assert(a_skybox, "EditorLevel skybox actor not found");

    MaterialParser materialParser;
    const std::shared_ptr<IMaterial>& spaceStars_material = materialParser.ParseMaterialDescriptor("SpaceStarsMaterial.m");
    sceneSp->RegisterMaterialInstance(spaceStars_material);

    MaterialPropertySetter::SetMaterialPropertyValue(spaceStars_material, sceneSp, "GT_DeltaSec", "gt_timeSec");
    MaterialPropertySetter::SetMaterialPropertyValue(spaceStars_material, "randomNormSeed", Random::Float() * 0.5f + 0.5f);

    auto billboardComponentCreator = std::make_shared<BillboardComponentCreator<BillboardComponent>>();
    const auto backgroundBillboardComponentData = std::make_shared<BillboardComponentData>(
        "c_spaceBackgroundBillboard",
        1.0f,
        true,
        glm::vec3(0.0f, 0.0f, 1.0f),
        0.0f,
        false,
        glm::vec3(1.0f),
        spaceStars_material,
        [](const glm::mat4& viewMatrix) { return glm::mat4(1); },
        [](const glm::mat4& projectionMatrix) { return glm::mat4(1); });
    const auto& billboardComponent = std::static_pointer_cast<BillboardComponent>(
        sceneSp->CreateComponent_GameThread(billboardComponentCreator, backgroundBillboardComponentData));
    billboardComponent->SetSortOrderValue(-100000);
    a_skybox->AddComponent(billboardComponent);

    const auto& a_light = sceneSp->GetActorByName("MainLightActor");
    ext_assert(a_light, "EditorLevel main light actor not found");
    std::static_pointer_cast<LightComponent>(a_light->GetComponentsByType<LightComponent>().front())->SetIsVisible(true);

    mLevelEditorController->OnLevelInit();
    mLevelEditorController->Initialize();

    mUiController->OnLevelInit();
}

void EditorLevel::PostLevelInit()
{
    Base::PostLevelInit();
    mLevelEditorController->OnPostLevelInit();
    mUiController->OnPostLevelInit();
}

void EditorLevel::PostPlayLevelFinished()
{
    Base::PostPlayLevelFinished();
    mLevelEditorController->PostPlayLevelFinished();
    mUiController->PostPlayLevelFinished();
}

void EditorLevel::InitLevel()
{
    Base::InitLevel();
    RunLuaBuildLevelScript();
    CreateScene();
}

void EditorLevel::UnloadLevel()
{
    if (mLevelEditorController) {
        mLevelEditorController->CleanUp();
        mLevelEditorController.reset();
    }

    if (mUiController) {
        mUiController->CleanUp();
        mUiController.reset();
    }
}

void EditorLevel::Tick(const float deltaTimeSec, const float playSpeed)
{
    if (mLevelEditorController) {
        mLevelEditorController->Tick(deltaTimeSec, playSpeed);
    }

    if (mUiController) {
        mUiController->Tick(deltaTimeSec, playSpeed);
    }
}

void EditorLevel::UnpausableTick(const float deltaTimeSec, const float playSpeed)
{
    if (mLevelEditorController) {
        mLevelEditorController->UnpausableTick(deltaTimeSec, playSpeed);
    }

    if (mUiController) {
        mUiController->UnpausableTick(deltaTimeSec, playSpeed);
    }
}

void EditorLevel::RestartLuaScripts()
{
    LogInfo("EditorLevel::RestartLuaScripts");
    if (mUiController) {
        mUiController->RestartLuaScripts();
    }
}
} // namespace Game
