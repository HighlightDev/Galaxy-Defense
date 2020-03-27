#include "SimpleLevel.h"

#include "Core/GameCore/FirstPersonCamera.h"
#include "Core/GameCore/ThirdPersonCamera.h"

#include "Core/GameCore/Components/BillboardComponent.h"
#include "Core/GameCore/Components/StaticMeshComponent.h"
#include "Core/GameCore/Components/SkeletalMeshComponent.h"
#include "Core/GameCore/Components/SkyboxComponent.h"
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

#include "Core/GraphicsCore/Material/PBRMaterial.h"
#include "Core/GraphicsCore/Material/WaterDynamicMaterial.h"
#include "Core/GraphicsCore/Material/SkyboxDynamicMaterial.h"
#include "Core/GraphicsCore/Shadow/ProjectedDirShadowInfo.h"
#include "Core/GameCore/Physics/PhysicsWorld.h"

#include <glm/vec3.hpp>

#include <LoggerMain.h>

using namespace Graphics;

namespace Labyrinth
{

   SimpleLevel::SimpleLevel(InterThreadCommunicationMgr& threadMgr)
      : Level(threadMgr)
   {
   }

   SimpleLevel::~SimpleLevel()
   {

   }

   void SimpleLevel::LoadLevel()
   {

      const auto& folderManager = Common::FolderManager::GetInstance();

      // test physics
      {
         auto object = mScene->mPhysicsWorld->LoadSimpleSkinWithPhysics(folderManager->GetModelPath() + "playerCube.obj");
         mScene->mPhysicsWorld->CreateBodyWithMass(50.0f, std::get<1>(object), true);

         auto collisionBody = mScene->mPhysicsWorld->LoadFloor();
         mScene->mPhysicsWorld->CreateBodyWithMass(1000, collisionBody, false);

         // Test for PBR
         {
            auto albedoTex = TexturePool::GetInstance()->GetOrAllocateResource(folderManager->GetAlbedoTexturePath() + "brick_mid.png");
            auto normalMapTex = TexturePool::GetInstance()->GetOrAllocateResource(folderManager->GetNormalMapPath() + "brick_nm_mid.png");

            StaticMeshComponentData mData(folderManager->GetModelPath() + "playerCube.obj", glm::vec3(0), glm::vec3(), glm::vec3(2.5f),
               std::make_shared<PBRMaterial>(albedoTex, normalMapTex, nullptr, nullptr, nullptr));

            std::shared_ptr<Actor> cubeActor = std::make_shared<Actor>("TestPhysicsActor", std::make_shared<SceneComponent>(std::move(glm::vec3(0, 50, 0)), std::move(glm::vec3(0)), std::move(glm::vec3(1))));
            auto component = mScene->CreateComponent_GameThread<StaticMeshComponent>(mData);
            mScene->AddComponentToActor_GameThread(cubeActor, component);
            cubeActor->GetRootComponent()->bIsPhysicsComponent = true;
            mScene->AllActors.push_back(cubeActor);

            // TODO: TEMP
            cubeActor->pWorld = mScene->mPhysicsWorld;
         }
      }

      // Dir light
      {
         auto directionalLightTextureAtlasRequest = TextureAtlasFactory::GetInstance()->AddTextureAtlasRequest(glm::ivec2(512, 512));
         ProjectedShadowInfo* dirLightInfo = new ProjectedDirShadowInfo(directionalLightTextureAtlasRequest);

         auto directionalLightTextureAtlasRequest1 = TextureAtlasFactory::GetInstance()->AddTextureAtlasRequest(glm::ivec2(512, 512));
         ProjectedShadowInfo* dirLightInfo1 = new ProjectedDirShadowInfo(directionalLightTextureAtlasRequest1);

         DirectionalLightComponentData mData(glm::vec3(0), glm::vec3(0.5f, -0.5f, 0), glm::vec3(0.2f, 0.2f, 0.2f),
            glm::vec3(0.68f, 0.5f, 0.5f), glm::vec3(0.7f, 0.7f, 0.7f), dirLightInfo);

         DirectionalLightComponentData mData1(glm::vec3(0), glm::vec3(-0.5f, -0.5f, 0), glm::vec3(0.2f, 0.2f, 0.2f),
            glm::vec3(0.68f, 0.5f, 0.5f), glm::vec3(0.7f, 0.7f, 0.7f), dirLightInfo1);

         std::shared_ptr<Actor> dirLightActor = std::make_shared<Actor>("Main lights", std::make_shared<SceneComponent>());

         auto dirLightComponent =  mScene->CreateComponent_GameThread<DirectionalLightComponent>(mData);
         mScene->AddComponentToActor_GameThread(dirLightActor, dirLightComponent);
         dirLightComponent = mScene->CreateComponent_GameThread<DirectionalLightComponent>(mData1);
         mScene->AddComponentToActor_GameThread(dirLightActor, dirLightComponent);
         mScene->AllActors.push_back(dirLightActor);
      }

      Log::LogMessages(LOG_INFO);

      // Water
      {
         auto normalTex = TexturePool::GetInstance()->GetOrAllocateResource(folderManager->GetNormalMapPath() + "water_normal.png");
         auto distortionTex = TexturePool::GetInstance()->GetOrAllocateResource(folderManager->GetDistortionTexturePath() + "water_dudv.png");
         WaterPlaneComponentData mData(glm::vec3(0), glm::vec3(0), glm::vec3(20), std::make_shared<WaterDynamicMaterial>(normalTex, distortionTex));
         std::shared_ptr<Actor> waterActor = std::make_shared<Actor>("Water", std::make_shared<SceneComponent>(std::move(glm::vec3(0)), std::move(glm::vec3(0)), std::move(glm::vec3(1))));
         auto waterComp = mScene->CreateComponent_GameThread<WaterPlaneComponent>(mData);
         mScene->AddComponentToActor_GameThread(waterActor, waterComp);
         mScene->AllActors.push_back(waterActor);
      }

#if 0
      // Ground

      {
         auto albedoTex = TexturePool::GetInstance()->GetOrAllocateResource(folderManager->GetAlbedoTexturePath() + "brick_mid.png");
         auto normalMapTex = TexturePool::GetInstance()->GetOrAllocateResource(folderManager->GetNormalMapPath() + "brick_nm_mid.png");

         StaticMeshComponentData mData(folderManager->GetModelPath() + "plane.obj", glm::vec3(0), glm::vec3(0), glm::vec3(50),
            std::make_shared<PBRMaterial>(albedoTex, normalMapTex, nullptr, nullptr, nullptr));
         std::shared_ptr<Actor> groundActor = std::make_shared<Actor>("Ground", std::make_shared<SceneComponent>(std::move(glm::vec3(0)), std::move(glm::vec3(0)), std::move(glm::vec3(1))));
         mScene->CreateAndAddComponent_GameThread<StaticMeshComponent>(mData, groundActor);
         mScene->AllActors.push_back(groundActor);
      }

#endif

      // Test for PBR
      {
         auto albedoTex1 = TexturePool::GetInstance()->GetOrAllocateResource(folderManager->GetAlbedoTexturePath() + "city_house_2_Col.png");
         auto normalMapTex1 = TexturePool::GetInstance()->GetOrAllocateResource(folderManager->GetNormalMapPath() + "city_house_2_Nor.png");
         auto specualrMapTex1 = TexturePool::GetInstance()->GetOrAllocateResource(folderManager->GetSpecularMapPath() + "city_house_2_Spec.png");

         float x_offset = 0, y_offset = 0.0f;

         StaticMeshComponentData mData(folderManager->GetModelPath() + "City_House_2_BI.obj", glm::vec3(0), glm::vec3(), glm::vec3(2.5f),
            std::make_shared<PBRMaterial>(albedoTex1, normalMapTex1, specualrMapTex1, nullptr, nullptr));

         std::shared_ptr<Actor> houseActor = std::make_shared<Actor>("House Actor", std::make_shared<SceneComponent>(std::move(glm::vec3(10)), std::move(glm::vec3(0)), std::move(glm::vec3(1))));
         auto staticComp = mScene->CreateComponent_GameThread<StaticMeshComponent>(mData);
         mScene->AddComponentToActor_GameThread(houseActor, staticComp);
         mScene->AllActors.push_back(houseActor);
      }

      // SKELETAL MESH
      {

         auto albedoTex = TexturePool::GetInstance()->GetOrAllocateResource(folderManager->GetAlbedoTexturePath() + "diffuse.png");
         auto normalMapTex = TexturePool::GetInstance()->GetOrAllocateResource(folderManager->GetNormalMapPath() + "dummy_nm.png");

         SkeletalMeshComponentData mData(folderManager->GetModelPath() + "model.dae", glm::vec3(0, 0, 0), glm::vec3(270, 0, 0), glm::vec3(0.5f),
            std::make_shared<PBRMaterial>(albedoTex, normalMapTex, nullptr, nullptr, nullptr));

         std::shared_ptr<Actor> skeletActor = std::make_shared<Actor>("Buddy", std::make_shared<SceneComponent>(std::move(glm::vec3(10)), std::move(glm::vec3(0)), std::move(glm::vec3(1))));
         auto skeletalComp = mScene->CreateComponent_GameThread<SkeletalMeshComponent>(mData);
         mScene->AddComponentToActor_GameThread(skeletActor, skeletalComp);

         InputComponentData inputComponentData;
         auto inputComp = mScene->CreateComponent_GameThread<InputComponent>(inputComponentData);
         mScene->AddComponentToActor_GameThread(skeletActor, inputComp);
         MovementComponentData movementComponentData(glm::vec3(0), GetCamera()->GetCameraName());
         auto movementComp = mScene->CreateComponent_GameThread<MovementComponent>(movementComponentData);
         mScene->AddComponentToActor_GameThread(skeletActor, movementComp);

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
            folderManager->GetCubemapTexturePath(), "Day/", "right.png", ",",
            folderManager->GetCubemapTexturePath(), "Day/", "left.png", ",",
            folderManager->GetCubemapTexturePath(), "Day/", "top.png", ",",
            folderManager->GetCubemapTexturePath(), "Day/", "bottom.png", ",",
            folderManager->GetCubemapTexturePath(), "Day/", "back.png", ",",
            folderManager->GetCubemapTexturePath(), "Day/", "front.png");
         auto dTexPath = StringStreamWrapper::FlushString();

         std::shared_ptr<ITexture> dayTex = TexturePool::GetInstance()->GetOrAllocateResource(dTexPath);

         SkyboxComponentData mData(glm::vec3(140.0f), std::make_shared<SkyboxDynamicMaterial>(dayTex, nullptr));
         std::shared_ptr<Actor> skyboxActor = std::make_shared<Actor>("Skybox Actor", std::make_shared<SceneComponent>(std::move(glm::vec3(0)), std::move(glm::vec3(0)), std::move(glm::vec3(1))));
         auto skyboxComp = mScene->CreateComponent_GameThread<SkyboxComponent>(mData);
         mScene->AddComponentToActor_GameThread(skyboxActor, skyboxComp);
         mScene->AllActors.push_back(skyboxActor);
      }

      TextureAtlasFactory::GetInstance()->AllocateAtlasSpace();
   }
}
