#include "SimpleLevel.h"

#include "Core/GameCore/FirstPersonCamera.h"
#include "Core/GameCore/ThirdPersonCamera.h"

#include "Core/GameCore/Components/PrimitiveComponents/BillboardComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/SkeletalMeshComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/SkyboxComponent.h"
#include "Core/GameCore/Components/PhysicsComponents/PhysicsComponent.h"
#include "Core/GameCore/Components/PointLightComponent.h"
#include "Core/GameCore/Components/InputComponent.h"
#include "Core/GameCore/Components/MovementComponent.h"

#include "Core/GameCore/Components/ComponentData/BillboardComponentData.h"
#include "Core/GameCore/Components/ComponentData/MeshComponentData.h"
#include "Core/GameCore/Components/ComponentData/SkyboxComponentData.h"
#include "Core/GameCore/Components/ComponentData/PointLightComponentData.h"
#include "Core/GameCore/Components/ComponentData/InputComponentData.h"
#include "Core/GameCore/Components/ComponentData/MovementComponentData.h"
#include "Core/GameCore/Components/ComponentData/MovementComponentData.h"
#include "Core/GameCore/Components/ComponentData/PhysicsComponentData.h"

#include "Core/GameCore/GlobalSettings.h"
#include "Core/IoCore/AsyncLoaderCore/ResourceMap.h"

#include "Core/GraphicsCore/Material/WaterDynamicMaterial.h"
#include "Core/GraphicsCore/Material/SkyboxDynamicMaterial.h"

#include "Core/GameCore/Physics/PhysicsWorld.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/PhyBoxShape.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/PhyCapsuleShape.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/DynamicCharacterController.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/RigidBodyController.h"

#include "Core/GameCore/ScriptingCore/LuaScriptExecutor_EngineObjectsCreator.h"
#include "Core/GraphicsCore/Material/MaterialParser.h"

#include <glm/vec3.hpp>
#include <LogInterface.h>

#include "PlayerActor.h"

using namespace Graphics;
using namespace EnginePhysics;
using namespace IO;

namespace Labyrinth
{

#define GET_REL_PATH_TO_FILE(fileName) (IO::FolderManager::GetInstance()->GetDirectoryRelativePathByFileName(fileName))

   SimpleLevel::SimpleLevel(InterThreadCommunicationMgr& threadMgr)
      : Level(threadMgr)
   {
   }

   SimpleLevel::~SimpleLevel()
   {

   }

   void SimpleLevel::RunLuaBuildLevelScript()
   {
      LuaScriptExecutor_EngineObjectsCreator mLuaLevelBuilder = LuaScriptExecutor_EngineObjectsCreator(mScene, "createTestLevel.lua");

      mLuaLevelBuilder.RegisterCallbacks();
      mLuaLevelBuilder.RunScript();
   }

   void SimpleLevel::PreConstructorInitialize()
   {
      Base::PreConstructorInitialize();

      const auto folderManager = IO::FolderManager::GetInstance();

#define ALLOC_RES_ASYNC(path) ResourceMap::GetInstance()->AllocateAsync(path)

      folderManager->CreateFilePathMap(folderManager->GetAlbedoTexturePath());
      folderManager->CreateFilePathMap(folderManager->GetNormalMapPath());
      folderManager->CreateFilePathMap(folderManager->GetSpecularMapPath());
      folderManager->CreateFilePathMap(folderManager->GetCubemapTexturePath());
      folderManager->CreateFilePathMap(folderManager->GetModelPath());
      folderManager->CreateFilePathMap(folderManager->GetMaterialPath());
      folderManager->CreateFilePathMap(folderManager->GetScriptPath());

      ALLOC_RES_ASYNC(GET_REL_PATH_TO_FILE("brick_mid.png"));
      ALLOC_RES_ASYNC(GET_REL_PATH_TO_FILE("brick_nm_mid.png"));
      ALLOC_RES_ASYNC(GET_REL_PATH_TO_FILE("city_house_2_Col.png"));
      ALLOC_RES_ASYNC(GET_REL_PATH_TO_FILE("city_house_2_Nor.png"));
      ALLOC_RES_ASYNC(GET_REL_PATH_TO_FILE("city_house_2_Spec.png"));
      ALLOC_RES_ASYNC(GET_REL_PATH_TO_FILE("diffuse.png"));
      ALLOC_RES_ASYNC(GET_REL_PATH_TO_FILE("dummy_nm.png"));
      ALLOC_RES_ASYNC(GET_REL_PATH_TO_FILE("dayRight.png"));
      ALLOC_RES_ASYNC(GET_REL_PATH_TO_FILE("dayLeft.png"));
      ALLOC_RES_ASYNC(GET_REL_PATH_TO_FILE("dayTop.png"));
      ALLOC_RES_ASYNC(GET_REL_PATH_TO_FILE("dayBottom.png"));
      ALLOC_RES_ASYNC(GET_REL_PATH_TO_FILE("dayBack.png"));
      ALLOC_RES_ASYNC(GET_REL_PATH_TO_FILE("dayFront.png"));

      ALLOC_RES_ASYNC(GET_REL_PATH_TO_FILE("playerCube.obj"));
      ALLOC_RES_ASYNC(GET_REL_PATH_TO_FILE("City_House_2_BI.obj"));
      ALLOC_RES_ASYNC(GET_REL_PATH_TO_FILE("model.dae"));
      ALLOC_RES_ASYNC(GET_REL_PATH_TO_FILE("player_walk.fbx"));
      //ALLOC_RES_ASYNC(GET_REL_PATH_TO_FILE("tina.fbx"));

#undef ALLOC_RES_ASYNC
   }

   void SimpleLevel::PostConstructorInitialize()
   {
      Base::PostConstructorInitialize();

      ResourceMap::DeleteInstance();
      TextureAtlasFactory::GetInstance()->AllocateAtlasSpace();
   }

   void SimpleLevel::LoadLevel()
   {
      ResourceMap::GetInstance()->WaitUntilResourcesLoad();

      RunLuaBuildLevelScript();
    
#if 0
      // Water
      {
         auto normalTex = TexturePool::GetInstance()->GetOrAllocateResource(GET_REL_PATH_TO_FILE("water_normal.png");
         auto distortionTex = TexturePool::GetInstance()->GetOrAllocateResource(folderManager->GetDistortionTexturePath() + "water_dudv.png");
         WaterPlaneComponentData mData(glm::vec3(0), glm::vec3(0), glm::vec3(20), std::make_shared<WaterDynamicMaterial>(normalTex, distortionTex));

         std::shared_ptr<Actor> waterActor = std::make_shared<Actor>("Water", std::make_shared<SceneComponent>(std::move(glm::vec3(0)), std::move(glm::vec3(0)), std::move(glm::vec3(1))));

         auto waterComp = mScene->CreateComponent_GameThread<ComponentMetaType::WaterPlane, WaterPlaneComponent>(mData);
         waterActor->AddComponent(waterComp);

         mScene->AllActors.push_back(waterActor);
      }
#endif

      // SKELETAL MESH
      {

         auto albedoTex = TexturePool::GetInstance()->GetOrAllocateResource(GET_REL_PATH_TO_FILE("dummy_nm.png"));
         auto normalMapTex = TexturePool::GetInstance()->GetOrAllocateResource(GET_REL_PATH_TO_FILE("dummy_nm.png"));

         IMaterial* pbrMaterial = MaterialParser::ParseMaterialDescriptor(GET_REL_PATH_TO_FILE("Pbs.m"));
         MaterialPropertySetter::SetMaterialPropertyValue(pbrMaterial, "albedo", albedoTex);
         MaterialPropertySetter::SetMaterialPropertyValue(pbrMaterial, "normalMap", normalMapTex);
         MaterialPropertySetter::SetMaterialPropertyValue(pbrMaterial, "uvScale", 1.0f);

         MeshComponentData mData(GET_REL_PATH_TO_FILE("player_walk.fbx"), glm::vec3(0, -0.6f, 0), glm::vec3(0, 0, 0), glm::vec3(3),
            GET_REL_PATH_TO_FILE("skeletComponentAction.lua"), pbrMaterial);

         std::shared_ptr<PlayerActor> skeletActor = std::make_shared<PlayerActor>("Buddy", std::make_shared<SceneComponent>(
            std::move(glm::vec3(10, 50, 10)), std::move(glm::vec3(0)), std::move(glm::vec3(1))));
         auto skeletalComp = mScene->CreateComponent_GameThread<ComponentMetaType::SkeletalMesh, SkeletalMeshComponent>(mData);
         skeletActor->AddComponent(skeletalComp);

         InputComponentData inputComponentData;
         auto inputComp = mScene->CreateComponent_GameThread<ComponentMetaType::Input, InputComponent>(inputComponentData);
         skeletActor->AddComponent(inputComp);
         MovementComponentData movementComponentData(glm::vec3(0), GetCamera()->GetCameraName());
         auto movementComp = mScene->CreateComponent_GameThread<ComponentMetaType::Movement, MovementComponent>(movementComponentData);
         skeletActor->AddComponent(movementComp);

         PhysicsDescriptor* playerPhysDesc = new DynamicCharacterController(mScene->mPhysicsWorld, 1, 2.5, 10, 1.0f);
         mScene->mPhysicsWorld->AddPhysDescriptor(playerPhysDesc);
         std::shared_ptr<Component> playerPhysComponent = mScene->CreateComponent_GameThread<ComponentMetaType::Physics, CharacterPhysicsComponent>(PhysicsComponentData(playerPhysDesc));

         skeletActor->AddComponent(playerPhysComponent);

         mScene->m_playerController.SetPlayerActor(skeletActor);

         mScene->AllActors.push_back(skeletActor);

         ICamera* camera = GetCamera();

         if (camera->GetCameraType() == ICamera::CameraType::THIRD_PERSON)
         {
            static_cast<ThirdPersonCamera*>(camera)->SetThirdPersonTarget(skeletActor);
         }
      }

      // SKYBOX
      {
         StringStreamWrapper::ToString(
            GET_REL_PATH_TO_FILE("dayRight.png"), ",",
            GET_REL_PATH_TO_FILE("dayLeft.png"), ",",
            GET_REL_PATH_TO_FILE("dayTop.png"), ",",
            GET_REL_PATH_TO_FILE("dayBottom.png"), ",",
            GET_REL_PATH_TO_FILE("dayBack.png"), ",",
            GET_REL_PATH_TO_FILE("dayFront.png"));
         auto dTexPath = StringStreamWrapper::FlushString();

         std::shared_ptr<ITexture> dayTex = TexturePool::GetInstance()->GetOrAllocateResource(dTexPath);

         SkyboxComponentData mData(glm::vec3(140.0f), std::make_shared<SkyboxDynamicMaterial>(dayTex, nullptr));
         std::shared_ptr<Actor> skyboxActor = std::make_shared<Actor>("Skybox Actor", std::make_shared<SceneComponent>(std::move(glm::vec3(0)), std::move(glm::vec3(0)), std::move(glm::vec3(1))));
         auto skyboxComp = mScene->CreateComponent_GameThread<ComponentMetaType::Skybox, SkyboxComponent>(mData);
         skyboxActor->AddComponent(skyboxComp);
         mScene->AllActors.push_back(skyboxActor);
      }
   }
#undef GET_REL_PATH_TO_FILE
}
