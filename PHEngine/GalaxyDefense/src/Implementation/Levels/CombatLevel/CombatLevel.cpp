#include "CombatLevel.h"

#include "Core/CommonCore/JsonHelper.h"
#include "Core/CommonCore/Random.h"
#include "Core/GameCore/BoundingBox3D.h"
#include "Core/GameCore/Components/ComponentCreators/BillboardComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/InputComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/InstancedStaticMeshComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/MovementComponentCreator.h"
#include "Core/GameCore/Components/ComponentData/BillboardComponentData.h"
#include "Core/GameCore/Components/ComponentData/InstancedMeshComponentData.h"
#include "Core/GameCore/Components/ComponentData/MeshComponentData.h"
#include "Core/GameCore/Components/ComponentData/PhysicsComponentData.h"
#include "Core/GameCore/Components/InputComponent.h"
#include "Core/GameCore/Components/LightComponent.h"
#include "Core/GameCore/Components/NoPhysicsMovementComponent.h"
#include "Core/GameCore/Components/PhysicsComponents/GhostPhysicsComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/BillboardComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/InstancedStaticMeshComponent.h"
#include "Core/GameCore/Event/GameThreadEventDispatcher.h"
#include "Core/GameCore/Event/LuaThreadEventDispatcher.h"
#include "Core/GameCore/GUI/UiElements/Transform2D/BoundingBox2D.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/GhostController.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/CollisionSphereShape.h"
#include "Core/GameCore/ThirdPersonCamera.h"
#include "Core/GameCore/Tweener/BindingAttachmentBuilder.h"
#include "Core/GameCore/Tweener/Tweener.h"
#include "Core/GameCore/Tweener/TweenerParser.h"
#include "Core/GraphicsCore/Material/MaterialParser.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialPropertySetter.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPerspectiveInfo.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"
#include "Core/IoCore/DisplayDeviceDataProvider.h"
#include "Core/IoCore/FileFacade.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Implementation/Controllers/CombatController.h"
#include "Implementation/Events/ChangeGameModeEvent.h"
#include "Implementation/Events/ElectroRayCollisionEvent.h"
#include "Implementation/Events/ElectroRaySphereContactCollisionEvent.h"
#include "Implementation/Events/LevelProgressChangedEvent.h"
#include "Implementation/Events/MainPlayerStatusChangedEvent.h"
#include "Implementation/GalaxySceneCamera.h"
#include "Implementation/Levels/LevelSerializationHelper.h"
#include "Implementation/LuaExecutors/LuaCombatLevelExecutor.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

using namespace IO;
using namespace Graphics;
using namespace EnginePhysics;
using namespace EngineCore;
using namespace EngineCore::Scripts;
using namespace EngineCore::GUI;

namespace Game {

CombatLevel::CombatLevel()
    : LevelBase("CombatLevel")
{
    Event::GameThreadEventDispatcher::GetInstance()
        ->RegisterEventsByType<
            Event::ElectroRayCollisionEvent,
            Event::ElectroRaySphereContactCollisionEvent,
            Event::MainPlayerStatusChangedEvent,
            Event::ChangeGameModeEvent,
            Event::LevelProgressChangedEvent>();
    Event::LuaThreadEventDispatcher::GetInstance()
        ->RegisterEventsByType<
            Event::LuaMainPlayerStatusChangedEvent,
            Event::LuaLevelProgressChangedEvent,
            Event::LuaChangeGameModeEvent>();
}

CombatLevel::~CombatLevel()
{
}

void CombatLevel::RunLuaBuildLevelScript()
{
    const auto sceneSp = mSceneWp.lock();
    assert(sceneSp);
    LuaCombatLevelExecutor mLuaLevelBuilder = LuaCombatLevelExecutor("CombatLvl.lua", mLvlProgressController);
    mLuaLevelBuilder.SetScene(sceneSp);
    mLuaLevelBuilder.SetLuaScriptProcessor(sceneSp->GetInterThreadCommunicationManager().GetLuaScriptProcessor());
    mLuaLevelBuilder.RegisterCallbacks();
    mLuaLevelBuilder.RunScript();
    mLuaLevelBuilder.StopScript();
}

void CombatLevel::PreLevelInit()
{
    const auto sceneSp = mSceneWp.lock();
    assert(sceneSp);
    Base::PreLevelInit();
    mCombatController = std::make_shared<CombatController>(sceneSp);
    mUiController = std::make_unique<CombatLevelUiController>(sceneSp, mLvlProgressController);
    mCombatController->OnPreLevelInit();
    mUiController->OnPreLevelInit();
}

LevelData CombatLevel::LoadLevelDataFromFile(const std::string& levelName) const
{
    const auto& sceneSp = mSceneWp.lock();
    assert(sceneSp);
    FileFacade fileReader;
    fileReader.OpenAndReadFile(levelName);
    const auto& lvlJsonStr = fileReader.GetFileSrc().front();

    LevelSerializationHelper lvlSerializationHelper;
    const auto lvlData = lvlSerializationHelper.RestoreLevelFromJsonString(lvlJsonStr);
    assert(lvlData.isDataValid());
    return lvlData;
}

void CombatLevel::CreateScene()
{
    const auto sceneSp = mSceneWp.lock();
    assert(sceneSp);

    const auto levelData = LoadLevelDataFromFile("TestLevelName");

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
        std::make_shared<ViewPerspectiveInfo>(glm::radians<float>(60.0f), 16.0f / 9.0f, 0.1f, 500.0f),
        38.88f,
        0.0f,
        150.0f);
    const auto& levelBoundary = BoundingBox3D(
        glm::vec3(0.0f),
        glm::vec3(
            std::abs(levelData.LevelBoundaryMax.x - levelData.LevelBoundaryMin.x),
            50.0f,
            std::abs(levelData.LevelBoundaryMax.y - levelData.LevelBoundaryMin.y)));
    spaceCamera->SetLevelBoundaries(levelBoundary);
    spaceCamera->SetMaxDistanceFromTargetToCamera(150.0f);
    spaceCamera->SetMinDistanceFromTargetToCamera(20.0f);
    spaceCamera->SetDistanceFromTargetToCamera(150.0f);
    sceneSp->RegisterMainCamera(spaceCamera);
    spaceCamera->SetThirdPersonTarget(a_sceneCenterActorDummy);

    const auto& a_light = sceneSp->GetActorByName("MainLightActor");
    assert(a_light);
    std::static_pointer_cast<LightComponent>(a_light->GetComponentsByType<LightComponent>().front())->SetIsVisible(true);

    const auto& a_skybox = sceneSp->GetActorByName("SkyboxActor");
    assert(a_skybox);

    MaterialParser materialParser;
    const std::shared_ptr<IMaterial>& spaceStars_material = materialParser.ParseMaterialDescriptor("SpaceStarsMaterial.m");
    sceneSp->RegisterMaterialInstance(spaceStars_material);

    MaterialPropertySetter::SetMaterialPropertyValue(spaceStars_material, sceneSp, "GT_DeltaSec", "gt_timeSec");

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

    mCombatController->InitFromLevelData(levelData);
    mCombatController->OnLevelInit();
    mUiController->OnLevelInit();

    // todo: temprorary solution just to test InstancedStaticMeshComponent
    const std::shared_ptr<IMaterial>& asteroidPbs_mat = materialParser.ParseMaterialDescriptor("AsteroidMaterial.m");
    sceneSp->RegisterMaterialInstance(asteroidPbs_mat);

    const std::string albedoName = "Asteroid_albedo.jpg";
    const std::string normalName = "Asteroid_normal.jpg";
    const std::string roughnessName = "Asteroid_roughness.jpg";
    const std::string metallicName = "Asteroid_metallic.jpg";

    const auto& albedo_tex = TexturePool::GetInstance()->GetOrAllocateResource(albedoName);
    const auto& normal_tex = TexturePool::GetInstance()->GetOrAllocateResource(normalName);
    const auto& roughness_tex = TexturePool::GetInstance()->GetOrAllocateResource(roughnessName);
    const auto& metallic_tex = TexturePool::GetInstance()->GetOrAllocateResource(metallicName);
    const float uvScale = 1.0f;

    MaterialPropertySetter::SetMaterialPropertyValue(asteroidPbs_mat, "albedo", albedo_tex);
    MaterialPropertySetter::SetMaterialPropertyValue(asteroidPbs_mat, "normalMap", normal_tex);
    MaterialPropertySetter::SetMaterialPropertyValue(asteroidPbs_mat, "roughnessMap", roughness_tex);
    MaterialPropertySetter::SetMaterialPropertyValue(asteroidPbs_mat, "metallicMap", metallic_tex);
    MaterialPropertySetter::SetMaterialPropertyValue(asteroidPbs_mat, "uvScale", uvScale);

    const auto& instancedMeshComponentCreator
        = std::make_shared<InstancedStaticMeshComponentCreator<InstancedStaticMeshComponent>>();
    const auto lvlDiffVec = levelBoundary.GetMax() - levelBoundary.GetMin();
    const float xStep = lvlDiffVec.x * (1.0f / 8.0f);
    const float zStep = lvlDiffVec.z * (1.0f / 8.0f);
    for (int i = 0; i < (8 * 8); ++i) {
        const auto d_ismesh = std::make_shared<InstancedMeshComponentData>(
            "c_instancedStaticMesh" + std::to_string(i),
            "asteroid.fbx",
            glm::vec3(),
            glm::vec3(),
            glm::vec3(1.0),
            asteroidPbs_mat);
        const auto& c_ismesh = std::static_pointer_cast<InstancedStaticMeshComponent>(
            sceneSp->CreateComponent_GameThread(instancedMeshComponentCreator, d_ismesh));
        const auto x = levelBoundary.GetMin().x + ((i % 8) * xStep);
        const auto z = levelBoundary.GetMin().z + ((i / 8) * zStep);
        const auto y = levelBoundary.GetOrigin().y - (Random::Float() * 10.0f);
        c_ismesh->SetScale(glm::vec3(35));
        c_ismesh->SetTranslation(glm::vec3(x, y, z));
        a_sceneCenterActorDummy->AddComponent(c_ismesh);
        a_sceneCenterActorDummy->SetScene(sceneSp);
    }
}

void CombatLevel::PostLevelInit()
{
    mCombatController->OnPostLevelInit();
    mUiController->OnPostLevelInit();
    Base::PostLevelInit();
}

void CombatLevel::PostPlayLevelFinished()
{
    mCombatController->PostPlayLevelFinished();
    mUiController->PostPlayLevelFinished();
    Base::PostPlayLevelFinished();
}

void CombatLevel::InitLevel()
{
    Base::InitLevel();
#if 0
      DeserializeLevel("test_serialize.xml");
#else
    RunLuaBuildLevelScript();
#endif
    CreateScene();
}

void CombatLevel::UnloadLevel()
{
    if (mUiController) {
        mUiController->CleanUp();
        mUiController.reset();
    }

    if (mCombatController) {
        mCombatController->CleanUp();
        mCombatController.reset();
    }
}

void CombatLevel::Tick(const float deltaTime)
{
    if (mUiController) {
        mUiController->Tick(deltaTime);
    }

    if (mCombatController) {
        mCombatController->Tick(deltaTime);
    }

    if (mLvlProgressController) {
        mLvlProgressController->Tick(deltaTime);
    }
}

void CombatLevel::UnpausableTick(const float deltaTime)
{
    if (mUiController) {
        mUiController->UnpausableTick(deltaTime);
    }

    if (mCombatController) {
        mCombatController->UnpausableTick(deltaTime);
    }
}

void CombatLevel::RestartLuaScripts()
{
    if (mUiController) {
        mUiController->RestartLuaScripts();
    }
}
} // namespace Game
