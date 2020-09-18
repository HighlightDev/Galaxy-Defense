#include "SimpleLevel.h"

#include "Core/GameCore/FirstPersonCamera.h"
#include "Core/GameCore/ThirdPersonCamera.h"

#include "Core/GameCore/Components/PrimitiveComponents/BillboardComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/StaticMeshComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/SkeletalMeshComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/SkyboxComponent.h"

#include "Core/GameCore/Components/PointLightComponent.h"
#include "Core/GameCore/Components/DirectionalLightComponent.h"

#include "Core/GameCore/Components/InputComponent.h"
#include "Core/GameCore/Components/MovementComponent.h"

#include "Core/GameCore/Components/ComponentData/BillboardComponentData.h"
#include "Core/GameCore/Components/ComponentData/StaticMeshComponentData.h"
#include "Core/GameCore/Components/ComponentData/SkeletalMeshComponentData.h"
#include "Core/GameCore/Components/ComponentData/SkyboxComponentData.h"
#include "Core/GameCore/Components/ComponentData/PointLightComponentData.h"
#include "Core/GameCore/Components/ComponentData/DirectionalLightComponentData.h"
#include "Core/GameCore/Components/ComponentData/InputComponentData.h"
#include "Core/GameCore/Components/ComponentData/MovementComponentData.h"
#include "Core/GameCore/Components/ComponentData/MovementComponentData.h"

#include "Core/GraphicsCore/Material/WaterDynamicMaterial.h"
#include "Core/GraphicsCore/Material/SkyboxDynamicMaterial.h"
#include "Core/GraphicsCore/Shadow/ProjectedDirShadowInfo.h"

#include "Core/GameCore/Physics/PhysicsWorld.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/PhyBoxShape.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/PhyCapsuleShape.h"
#include "Core/GameCore/Components/PhysicsComponents/PhysicsComponent.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/DynamicCharacterController.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/RigidBodyController.h"

#include <glm/vec3.hpp>

#include "Core/GameCore/GlobalSettings.h"
#include "Core/IoCore/AsyncLoaderCore/ResourceMap.h"

#include "PlayerSkeletalMeshComponent.h"
#include "PlayerActor.h"

#include <LogInterface.h>
#include "LuaScriptExecutor_LevelBuilder.h"
#include "Core/GraphicsCore/Material/MaterialParser.h"

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
      LuaScriptExecutor_LevelBuilder mLuaLevelBuilder = LuaScriptExecutor_LevelBuilder(mScene, "createTestLevel.lua");

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
      ALLOC_RES_ASYNC(GET_REL_PATH_TO_FILE("tina.fbx"));

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
      {
         // Test for PBR
         /*{
            auto albedoTex = TexturePool::GetInstance()->GetOrAllocateResource(folderManager->GetAlbedoTexturePath() + "brick_mid.png");
            auto normalMapTex = TexturePool::GetInstance()->GetOrAllocateResource(GET_REL_PATH_TO_FILE("brick_nm_mid.png");

            StaticMeshComponentData mData(folderManager->GetModelPath() + "tina.fbx", glm::vec3(0), glm::vec3(), glm::vec3(1),
               std::make_shared<PBRMaterial>(albedoTex, normalMapTex, nullptr, nullptr, nullptr, 1.0f));

            std::shared_ptr<PlayerActor> cubeActor = std::make_shared<PlayerActor>("TestPhysicsActor",
               std::make_shared<SceneComponent>(std::move(glm::vec3(0, 50, 0)), std::move(glm::vec3(17, 25 , 0)), std::move(glm::vec3(1))));
            auto component = mScene->CreateComponent_GameThread<ComponentMetaType::StaticMesh, StaticMeshComponent>(mData);
            cubeActor->AddComponent(component);

            PhyShapeBase* shape = new PhyBoxShape(glm::vec3(1.5f));
            PhysicsDescriptor* cubePhysDesc = new RigidBodyController(mScene->mPhysicsWorld, shape, 25.0f);
            mScene->mPhysicsWorld->AddPhysDescriptor(cubePhysDesc);
            std::shared_ptr<PhysicsComponent> cubePhysComponent = std::make_shared<PhysicsComponent>(cubePhysDesc);
            cubeActor->AddComponent(cubePhysComponent);

            mScene->AllActors.push_back(cubeActor);
         }*/
      }

      {
         /* {
             auto albedoTex = TexturePool::GetInstance()->GetOrAllocateResource(GET_REL_PATH_TO_FILE("dummy_nm.png");

             SkeletalMeshComponentData mData(folderManager->GetModelPath() + "tina.fbx", glm::vec3(0), glm::vec3(0, 0, 0), glm::vec3(5),
                std::make_shared<PBRMaterial>(albedoTex, nullptr, nullptr, nullptr, nullptr, 1.0f));

             std::shared_ptr<Actor> cubeActor = std::make_shared<Actor>("TestPhysicsActor1",
                std::make_shared<SceneComponent>(std::move(glm::vec3(0, 50, 0)), std::move(glm::vec3(17, 25, 0)), std::move(glm::vec3(1))));
             auto component = mScene->CreateComponent_GameThread<ComponentMetaType::SkeletalMesh, PlayerSkeletalMeshComponent>(mData);
             cubeActor->AddComponent(component);

             PhysicsDescriptor* physDesc = new DynamicCharacterController(mScene->mPhysicsWorld, 1, 2.5f, 10, 1.0f);
             mScene->mPhysicsWorld->AddPhysDescriptor(physDesc);
             std::shared_ptr<PhysicsComponent> cubePhysComponent = std::make_shared<CharacterPhysicsComponent>(physDesc);
             cubeActor->AddComponent(cubePhysComponent);

             mScene->AllActors.push_back(cubeActor);
          }*/
      }

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

#if 1
      // Ground

      {
         std::shared_ptr<Actor> groundActor = std::make_shared<Actor>("Ground", std::make_shared<SceneComponent>(glm::vec3(0), glm::vec3(0), glm::vec3(1)));
         mScene->AllActors.push_back(groundActor);

         auto albedoTex = TexturePool::GetInstance()->GetOrAllocateResource(GET_REL_PATH_TO_FILE("brick_mid.png"));
         auto normalMapTex = TexturePool::GetInstance()->GetOrAllocateResource(GET_REL_PATH_TO_FILE("brick_nm_mid.png"));

         IMaterial* pbrMaterial = MaterialParser::ParseMaterialDescriptor(GET_REL_PATH_TO_FILE("Pbs.m"));
         MaterialPropertySetter::SetMaterialPropertyValue(pbrMaterial, "albedo", albedoTex);
         MaterialPropertySetter::SetMaterialPropertyValue(pbrMaterial, "normalMap", normalMapTex);
         MaterialPropertySetter::SetMaterialPropertyValue(pbrMaterial, "uvScale", 10.0f);
      
         StaticMeshComponentData mData(GET_REL_PATH_TO_FILE("playerCube.obj"), glm::vec3(0, 0, 0), glm::vec3(0), glm::vec3(50, 1, 50), std::shared_ptr<IMaterial>(pbrMaterial));
         auto floorComponent = mScene->CreateComponent_GameThread<ComponentMetaType::StaticMesh, StaticMeshComponent>(mData);
         groundActor->AddComponent(floorComponent);

         PhyShapeBase* shape = new PhyBoxShape(glm::vec3(50, 1, 50));
         PhysicsDescriptor* floorPhysDesc = new RigidBodyController(mScene->mPhysicsWorld, shape, 0.0f);
         mScene->mPhysicsWorld->AddPhysDescriptor(floorPhysDesc);
         std::shared_ptr<PhysicsComponent> floorPhysComponent = std::make_shared<PhysicsComponent>(floorPhysDesc);

         groundActor->AddComponent(floorPhysComponent);
      }

      // Ground

      {
         auto albedoTex = TexturePool::GetInstance()->GetOrAllocateResource(GET_REL_PATH_TO_FILE("brick_mid.png"));
         auto normalMapTex = TexturePool::GetInstance()->GetOrAllocateResource(GET_REL_PATH_TO_FILE("brick_nm_mid.png"));

         IMaterial* pbrMaterial = MaterialParser::ParseMaterialDescriptor(GET_REL_PATH_TO_FILE("Pbs.m"));
         MaterialPropertySetter::SetMaterialPropertyValue(pbrMaterial, "albedo", albedoTex);
         MaterialPropertySetter::SetMaterialPropertyValue(pbrMaterial, "normalMap", normalMapTex);
         MaterialPropertySetter::SetMaterialPropertyValue(pbrMaterial, "uvScale", 1.0f);

         StaticMeshComponentData mData(GET_REL_PATH_TO_FILE("playerCube.obj"), glm::vec3(0, 0, 0), glm::vec3(0), glm::vec3(8, 1, 8),
            std::shared_ptr<IMaterial>(pbrMaterial));
         std::shared_ptr<Actor> groundActor = std::make_shared<Actor>("Ground", std::make_shared<SceneComponent>(
            std::move(glm::vec3(0, 10, 0)), std::move(glm::vec3(0)), std::move(glm::vec3(1))));

         auto floorComponent = mScene->CreateComponent_GameThread<ComponentMetaType::StaticMesh, StaticMeshComponent>(mData);
         groundActor->AddComponent(floorComponent);

         PhyShapeBase* shape = new PhyBoxShape(glm::vec3(8, 1, 8));
         PhysicsDescriptor* floorPhysDesc = new RigidBodyController(mScene->mPhysicsWorld, shape, 0.0f);
         mScene->mPhysicsWorld->AddPhysDescriptor(floorPhysDesc);
         std::shared_ptr<PhysicsComponent> floorPhysComponent = std::make_shared<PhysicsComponent>(floorPhysDesc);

         groundActor->AddComponent(floorPhysComponent);

         mScene->AllActors.push_back(groundActor);
      }

#endif

      // Test for PBR
      {
         auto albedoTex1 = TexturePool::GetInstance()->GetOrAllocateResource(GET_REL_PATH_TO_FILE("city_house_2_Col.png"));
         auto normalMapTex1 = TexturePool::GetInstance()->GetOrAllocateResource(GET_REL_PATH_TO_FILE("city_house_2_Nor.png"));
         auto specualrMapTex1 = TexturePool::GetInstance()->GetOrAllocateResource(GET_REL_PATH_TO_FILE("city_house_2_Spec.png"));

         IMaterial* pbrMaterial = MaterialParser::ParseMaterialDescriptor(GET_REL_PATH_TO_FILE("Pbs.m"));
         MaterialPropertySetter::SetMaterialPropertyValue(pbrMaterial, "albedo", albedoTex1);
         MaterialPropertySetter::SetMaterialPropertyValue(pbrMaterial, "normalMap", normalMapTex1);
         MaterialPropertySetter::SetMaterialPropertyValue(pbrMaterial, "metallicMap", specualrMapTex1);
         MaterialPropertySetter::SetMaterialPropertyValue(pbrMaterial, "uvScale", 1.0f);

         StaticMeshComponentData mData(GET_REL_PATH_TO_FILE("City_House_2_BI.obj"), glm::vec3(0, -2.5f, 0), glm::vec3(), glm::vec3(2.5f),
            std::shared_ptr<IMaterial>(pbrMaterial));
         auto staticComp = mScene->CreateComponent_GameThread<ComponentMetaType::StaticMesh, StaticMeshComponent>(mData);

         std::shared_ptr<Actor> houseActor = std::make_shared<Actor>("House Actor", std::make_shared<SceneComponent>(std::move(glm::vec3(0, 20, 0)), std::move(glm::vec3(0)), std::move(glm::vec3(1))));

         houseActor->AddComponent(staticComp);

         PhyShapeBase* shape = new PhyBoxShape(glm::vec3(6, 6.5f, 6));

         PhysicsDescriptor* housePhysDesc = new RigidBodyController(mScene->mPhysicsWorld, shape, 125.0f);
         mScene->mPhysicsWorld->AddPhysDescriptor(housePhysDesc);
         std::shared_ptr<PhysicsComponent> housePhysComponent = std::make_shared<PhysicsComponent>(housePhysDesc);
         houseActor->AddComponent(housePhysComponent);

         mScene->AllActors.push_back(houseActor);
      }

      // SKELETAL MESH
      {

         auto albedoTex = TexturePool::GetInstance()->GetOrAllocateResource(GET_REL_PATH_TO_FILE("dummy_nm.png"));
         auto normalMapTex = TexturePool::GetInstance()->GetOrAllocateResource(GET_REL_PATH_TO_FILE("dummy_nm.png"));

         IMaterial* pbrMaterial = MaterialParser::ParseMaterialDescriptor(GET_REL_PATH_TO_FILE("Pbs.m"));
         MaterialPropertySetter::SetMaterialPropertyValue(pbrMaterial, "albedo", albedoTex);
         MaterialPropertySetter::SetMaterialPropertyValue(pbrMaterial, "normalMap", normalMapTex);
         MaterialPropertySetter::SetMaterialPropertyValue(pbrMaterial, "uvScale", 1.0f);

         SkeletalMeshComponentData mData(GET_REL_PATH_TO_FILE("player_walk.fbx"), glm::vec3(0, -0.6f, 0), glm::vec3(0, 0, 0), glm::vec3(3),
            std::shared_ptr<IMaterial>(pbrMaterial));

         std::shared_ptr<PlayerActor> skeletActor = std::make_shared<PlayerActor>("Buddy", std::make_shared<SceneComponent>(
            std::move(glm::vec3(10, 50, 10)), std::move(glm::vec3(0)), std::move(glm::vec3(1))));
         auto skeletalComp = mScene->CreateComponent_GameThread<ComponentMetaType::SkeletalMesh, PlayerSkeletalMeshComponent>(mData);
         skeletActor->AddComponent(skeletalComp);

         InputComponentData inputComponentData;
         auto inputComp = mScene->CreateComponent_GameThread<ComponentMetaType::Input, InputComponent>(inputComponentData);
         skeletActor->AddComponent(inputComp);
         MovementComponentData movementComponentData(glm::vec3(0), GetCamera()->GetCameraName());
         auto movementComp = mScene->CreateComponent_GameThread<ComponentMetaType::Movement, MovementComponent>(movementComponentData);
         skeletActor->AddComponent(movementComp);

         PhysicsDescriptor* playerPhysDesc = new DynamicCharacterController(mScene->mPhysicsWorld, 1, 2.5, 10, 1.0f);
         mScene->mPhysicsWorld->AddPhysDescriptor(playerPhysDesc);
         std::shared_ptr<PhysicsComponent> playerPhysComponent = std::make_shared<CharacterPhysicsComponent>(playerPhysDesc);

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
