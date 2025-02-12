#include "EditorLevel.h"

#include "Core/GameCore/BoundingBox3D.h"
#include "Core/GameCore/Components/ComponentCreators/BillboardComponentCreator.h"
#include "Core/GameCore/Components/ComponentData/BillboardComponentData.h"
#include "Core/GameCore/Components/LightComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/BillboardComponent.h"
#include "Core/GameCore/Event/GameThreadEventDispatcher.h"
#include "Core/GameCore/Event/LuaThreadEventDispatcher.h"
#include "Core/GameCore/GUI/UiElements/Transform2D/BoundingBox2D.h"
#include "Core/GameCore/ScriptingCore/LuaScriptExecutors/LuaEngineScriptExecutor.h"
#include "Core/GraphicsCore/Material/MaterialParser.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialPropertySetter.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPerspectiveInfo.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"
#include "Core/IoCore/DisplayDeviceDataProvider.h"
#include "Implementation/Events/ChangeEditModeEvent.h"
#include "Implementation/GalaxySceneCamera.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

using namespace IO;
using namespace EngineCore;
using namespace EngineCore::Scripts;
using namespace EngineCore::GUI;
using namespace Graphics;

namespace Game {

EditorLevel::EditorLevel()
    : LevelBase("EditorLevel")
{
    Event::GameThreadEventDispatcher::GetInstance()->RegisterEventsByType<Event::ChangeEditModeEvent>();
    Event::LuaThreadEventDispatcher::GetInstance()->RegisterEventsByType<Event::ChangeEditModeEvent>();
}

EditorLevel::~EditorLevel()
{
}

void EditorLevel::RunLuaBuildLevelScript()
{
    const auto sceneSp = mSceneWp.lock();
    assert(sceneSp);
    LuaEngineScriptExecutor mLuaLevelBuilder = LuaEngineScriptExecutor("LevelCreators/EditorLvl.lua");
    mLuaLevelBuilder.SetScene(sceneSp);
    mLuaLevelBuilder.SetLuaScriptProcessor(sceneSp->GetInterThreadCommunicationManager().GetLuaScriptProcessor());
    mLuaLevelBuilder.RegisterCallbacks();
    mLuaLevelBuilder.RunScript();
    mLuaLevelBuilder.StopScript();
}

void EditorLevel::PreLevelInit()
{
    const auto sceneSp = mSceneWp.lock();
    assert(sceneSp);
    Base::PreLevelInit();
    mLevelEditorController = std::make_shared<LevelEditorController>(sceneSp);
    mLevelEditorController->OnPreLevelInit();

    mUiController = std::make_unique<LevelEditorUiController>(sceneSp);
    mUiController->OnPreLevelInit();
}

void EditorLevel::CreateScene()
{
    const auto sceneSp = mSceneWp.lock();
    assert(sceneSp);

    const auto& a_sceneCenterActorDummy = sceneSp->GetActorByName("SceneCenterActorDummy");
    assert(a_sceneCenterActorDummy);
    const auto& spaceCamera = std::make_shared<GalaxySceneCamera>(
        "LevelMainCamera",
        eCameraType::MAIN_THIRD_PERSON_CAMERA,
        sceneSp,
        ViewPortInfo(
            0,
            0,
            DisplayDeviceDataProvider::GetInstance()->GetWindowWidth(),
            DisplayDeviceDataProvider::GetInstance()->GetWindowHeight()),
        std::make_shared<ViewPerspectiveInfo>(glm::radians<float>(60.0f), 1.0f, 0.1f, 500.0f),
        60.0f,
        0.0f,
        200.0f);

    constexpr float c_levelExtent = 60.0f;
    spaceCamera->SetLevelBoundaries(BoundingBox3D(glm::vec3(), glm::vec3(c_levelExtent, 5.0f, c_levelExtent)));
    spaceCamera->SetMaxDistanceFromTargetToCamera(200.0f);
    spaceCamera->SetMinDistanceFromTargetToCamera(50.0f);
    spaceCamera->SetDistanceFromTargetToCamera(200.0f);
    sceneSp->RegisterMainCamera(spaceCamera);
    spaceCamera->SetThirdPersonTarget(a_sceneCenterActorDummy);

    const auto& a_skybox = sceneSp->GetActorByName("SkyboxActor");
    assert(a_skybox);

    MaterialParser materialParser;
    const std::shared_ptr<IMaterial>& spaceStars_material = materialParser.ParseMaterialDescriptor("SpaceStarsMaterial.m");
    sceneSp->RegisterMaterialInstance(spaceStars_material);

    MaterialPropertySetter::SetMaterialPropertyValue(spaceStars_material, sceneSp, "GT_DeltaSec", "gt_timeSec");
    MaterialPropertySetter::SetMaterialPropertyValue(spaceStars_material, sceneSp, "ScreenResolution", "screenResolution");

    auto billboardComponentCreator = std::make_shared<BillboardComponentCreator<BillboardComponent>>();
    const auto backgroundBillboardComponentData = std::make_shared<BillboardComponentData>(
        "c_spaceBackgroundBillboard",
        1.0f,
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(1.0f),
        spaceStars_material,
        [](const glm::mat4& viewMatrix) { return glm::mat4(1); },
        [](const glm::mat4& projectionMatrix) { return glm::mat4(1); });
    const auto& billboardComponent = std::static_pointer_cast<BillboardComponent>(
        sceneSp->CreateComponent_GameThread(billboardComponentCreator, backgroundBillboardComponentData));
    billboardComponent->SetSortOrderValue(-100000);
    a_skybox->AddComponent(billboardComponent);

    const auto& a_light = sceneSp->GetActorByName("MainLightActor");
    assert(a_light);
    std::static_pointer_cast<LightComponent>(a_light->GetComponentsByType<LightComponent>().front())->SetIsVisible(true);

    mLevelEditorController->OnLevelInit();
    mLevelEditorController->SetLevelAreaBoundingBox(BoundingBox2D<glm::vec2>(glm::vec2(), glm::vec2(c_levelExtent)));

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

void EditorLevel::Tick(const float deltaTime)
{
    if (mLevelEditorController) {
        mLevelEditorController->Tick(deltaTime);
    }

    if (mUiController) {
        mUiController->Tick(deltaTime);
    }
}

void EditorLevel::UnpausableTick(const float deltaTime)
{
    if (mLevelEditorController) {
        mLevelEditorController->UnpausableTick(deltaTime);
    }

    if (mUiController) {
        mUiController->UnpausableTick(deltaTime);
    }
}
} // namespace Game
