#include "SimpleLevel.h"

#include "Core/GameCore/FirstPersonCamera.h"
#include "Core/GameCore/ThirdPersonCamera.h"

#include "Core/GameCore/Components/PrimitiveComponents/BillboardComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/StaticMeshComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/SkeletalMeshComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/SkyboxComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/PhysicsShapeDebugRenderComponent.h"

#include "Core/GameCore/Components/PointLightComponent.h"
#include "Core/GameCore/Components/DirectionalLightComponent.h"

#include "Core/GameCore/Components/InputComponent.h"
#include "Core/GameCore/Components/MovementComponent.h"

#include "Core/GameCore/Components/ComponentData/BillboardComponentData.h"
#include "Core/GameCore/Components/ComponentData/StaticMeshComponentData.h"
#include "Core/GameCore/Components/ComponentData/SkeletalMeshComponentData.h"
#include "Core/GameCore/Components/ComponentData/SkyboxComponentData.h"
#include "Core/GameCore/Components/ComponentData/PhyShapeDebugComponentData.h"
#include "Core/GameCore/Components/ComponentData/PointLightComponentData.h"
#include "Core/GameCore/Components/ComponentData/DirectionalLightComponentData.h"
#include "Core/GameCore/Components/ComponentData/InputComponentData.h"
#include "Core/GameCore/Components/ComponentData/MovementComponentData.h"
#include "Core/GameCore/Components/ComponentData/MovementComponentData.h"

#include "Core/GraphicsCore/Material/PBRMaterial.h"
#include "Core/GraphicsCore/Material/WaterDynamicMaterial.h"
#include "Core/GraphicsCore/Material/SkyboxDynamicMaterial.h"
#include "Core/GraphicsCore/Shadow/ProjectedDirShadowInfo.h"

#include "Core/GameCore/Physics/PhysicsWorld.h"
#include "Core/GameCore/Components/PhysicsComponents/PhysicsDescriptors/Shapes/PhyBoxShape.h"
#include "Core/GameCore/Components/PhysicsComponents/PhysicsDescriptors/Shapes/PhyPlaneShape.h"
#include "Core/GameCore/Components/PhysicsComponents/PhysicsComponent.h"

#include <glm/vec3.hpp>

#include "Core/GameCore/GlobalSettings.h"

#include <LogInterface.h>

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

      {
         // Test for PBR
         {
            auto albedoTex = TexturePool::GetInstance()->GetOrAllocateResource(folderManager->GetAlbedoTexturePath() + "brick_mid.png");
            auto normalMapTex = TexturePool::GetInstance()->GetOrAllocateResource(folderManager->GetNormalMapPath() + "brick_nm_mid.png");

            StaticMeshComponentData mData(folderManager->GetModelPath() + "playerCube.obj", glm::vec3(0), glm::vec3(), glm::vec3(1),
               std::make_shared<PBRMaterial>(albedoTex, normalMapTex, nullptr, nullptr, nullptr));

            std::shared_ptr<Actor> cubeActor = std::make_shared<Actor>("TestPhysicsActor",
               std::make_shared<SceneComponent>(std::move(glm::vec3(0, 50, 0)), std::move(glm::vec3(17, 25 , 0)), std::move(glm::vec3(1))));
            auto component = mScene->CreateComponent_GameThread<StaticMeshComponent>(mData);
            cubeActor->AddComponent(component);

            PhyShapeBase* shape = new PhyBoxShape(glm::vec3(1.5f));

            PhysicsDescriptor* cubePhysDesc = new PhysicsDescriptor(shape, 25.0f);
            mScene->mPhysicsWorld->AddPhysDescriptor(cubePhysDesc);
            std::shared_ptr<PhysicsComponent> cubePhysComponent = std::make_shared<PhysicsComponent>(cubePhysDesc);
            cubeActor->AddComponent(cubePhysComponent);

            auto debugRenderPhysComp = mScene->CreateComponent_GameThread<PhysicsShapeDebugRenderComponent>(PhyShapeDebugComponentData(shape));
            cubeActor->AddComponent(debugRenderPhysComp);

            mScene->AllActors.push_back(cubeActor);
         }
      }

      // Dir light
      {
         auto directionalLightTextureAtlasRequest1 = TextureAtlasFactory::GetInstance()->AddTextureAtlasRequest(glm::ivec2(512, 512));
         ProjectedShadowInfo* shadowProjInfo1 = new ProjectedDirShadowInfo(directionalLightTextureAtlasRequest1,
         GlobalSettings::GetInstance()->GetShadowOrthoProjectionHalfExtent(),
            GlobalSettings::GetInstance()->GetShadowTransitionAreaLength());

         auto directionalLightTextureAtlasRequest2 = TextureAtlasFactory::GetInstance()->AddTextureAtlasRequest(glm::ivec2(512, 512));
         ProjectedShadowInfo* shadowProjInfo2 = new ProjectedDirShadowInfo(directionalLightTextureAtlasRequest2,
            GlobalSettings::GetInstance()->GetShadowOrthoProjectionHalfExtent(),
            GlobalSettings::GetInstance()->GetShadowTransitionAreaLength());

         DirectionalLightComponentData mData1(glm::vec3(0), glm::vec3(0.5f, -0.5f, 0), glm::vec3(0.2f, 0.2f, 0.2f),
            glm::vec3(1.68f, 1.5f, 1.5f), glm::vec3(2.7f, 2.7f, 2.7f), shadowProjInfo1);

         DirectionalLightComponentData mData2(glm::vec3(0), glm::vec3(-0.5f, -0.5f, 0), glm::vec3(0.2f, 0.2f, 0.2f),
            glm::vec3(1.68f, 1.5f, 1.5f), glm::vec3(0.7f, 0.7f, 0.7f), shadowProjInfo2);

         auto dirLightComponent1 =  mScene->CreateComponent_GameThread<DirectionalLightComponent>(mData1);
         auto dirLightComponent2 = mScene->CreateComponent_GameThread<DirectionalLightComponent>(mData2);

         std::shared_ptr<Actor> dirLightActor = std::make_shared<Actor>("Main lights", std::make_shared<SceneComponent>());

         dirLightActor->AddComponent(dirLightComponent1);
         dirLightActor->AddComponent(dirLightComponent2);
         mScene->AllActors.push_back(dirLightActor);
      }

#if 0
      // Water
      {
         auto normalTex = TexturePool::GetInstance()->GetOrAllocateResource(folderManager->GetNormalMapPath() + "water_normal.png");
         auto distortionTex = TexturePool::GetInstance()->GetOrAllocateResource(folderManager->GetDistortionTexturePath() + "water_dudv.png");
         WaterPlaneComponentData mData(glm::vec3(0), glm::vec3(0), glm::vec3(20), std::make_shared<WaterDynamicMaterial>(normalTex, distortionTex));

         std::shared_ptr<Actor> waterActor = std::make_shared<Actor>("Water", std::make_shared<SceneComponent>(std::move(glm::vec3(0)), std::move(glm::vec3(0)), std::move(glm::vec3(1))));

         auto waterComp = mScene->CreateComponent_GameThread<WaterPlaneComponent>(mData);
         waterActor->AddComponent(waterComp);

         mScene->AllActors.push_back(waterActor);
      }
#endif

#if 1
      // Ground

      {
         auto albedoTex = TexturePool::GetInstance()->GetOrAllocateResource(folderManager->GetAlbedoTexturePath() + "brick_mid.png");
         auto normalMapTex = TexturePool::GetInstance()->GetOrAllocateResource(folderManager->GetNormalMapPath() + "brick_nm_mid.png");

         StaticMeshComponentData mData(folderManager->GetModelPath() + "playerCube.obj", glm::vec3(0), glm::vec3(0), glm::vec3(50, 1, 50),
            std::make_shared<PBRMaterial>(albedoTex, normalMapTex, nullptr, nullptr, nullptr));
         std::shared_ptr<Actor> groundActor = std::make_shared<Actor>("Ground", std::make_shared<SceneComponent>(std::move(glm::vec3(0)), std::move(glm::vec3(0)), std::move(glm::vec3(1))));

         auto floorComponent = mScene->CreateComponent_GameThread<StaticMeshComponent>(mData);
         groundActor->AddComponent(floorComponent);

         PhysicsDescriptor* floorPhysDesc = new PhysicsDescriptor(new PhyPlaneShape(glm::vec3(0, 1, 0), 0), 0.0f);
         mScene->mPhysicsWorld->AddPhysDescriptor(floorPhysDesc);
         std::shared_ptr<PhysicsComponent> floorPhysComponent = std::make_shared<PhysicsComponent>(floorPhysDesc);
         groundActor->AddComponent(floorPhysComponent);

         mScene->AllActors.push_back(groundActor);
      }

#endif

      // Test for PBR
      {
         auto albedoTex1 = TexturePool::GetInstance()->GetOrAllocateResource(folderManager->GetAlbedoTexturePath() + "city_house_2_Col.png");
         auto normalMapTex1 = TexturePool::GetInstance()->GetOrAllocateResource(folderManager->GetNormalMapPath() + "city_house_2_Nor.png");
         auto specualrMapTex1 = TexturePool::GetInstance()->GetOrAllocateResource(folderManager->GetSpecularMapPath() + "city_house_2_Spec.png");

         StaticMeshComponentData mData(folderManager->GetModelPath() + "City_House_2_BI.obj", glm::vec3(0 ,-2.5f, 0), glm::vec3(), glm::vec3(2.5f),
            std::make_shared<PBRMaterial>(albedoTex1, normalMapTex1, specualrMapTex1, nullptr, nullptr));
         auto staticComp = mScene->CreateComponent_GameThread<StaticMeshComponent>(mData);

         std::shared_ptr<Actor> houseActor = std::make_shared<Actor>("House Actor", std::make_shared<SceneComponent>(std::move(glm::vec3(0, 20, 0)), std::move(glm::vec3(0)), std::move(glm::vec3(1))));
       
         houseActor->AddComponent(staticComp);

         PhyShapeBase* shape = new PhyBoxShape(glm::vec3(6, 6.5f, 6));

         PhysicsDescriptor* housePhysDesc = new PhysicsDescriptor(shape, 125.0f);
         mScene->mPhysicsWorld->AddPhysDescriptor(housePhysDesc);
         std::shared_ptr<PhysicsComponent> housePhysComponent = std::make_shared<PhysicsComponent>(housePhysDesc);
         houseActor->AddComponent(housePhysComponent);

         auto debugRenderPhysComp = mScene->CreateComponent_GameThread<PhysicsShapeDebugRenderComponent>(PhyShapeDebugComponentData(shape));
         houseActor->AddComponent(debugRenderPhysComp);

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
         skeletActor->AddComponent(skeletalComp);

         InputComponentData inputComponentData;
         auto inputComp = mScene->CreateComponent_GameThread<InputComponent>(inputComponentData);
         skeletActor->AddComponent(inputComp);
         MovementComponentData movementComponentData(glm::vec3(0), GetCamera()->GetCameraName());
         auto movementComp = mScene->CreateComponent_GameThread<MovementComponent>(movementComponentData);
         skeletActor->AddComponent(movementComp);

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
         skyboxActor->AddComponent(skyboxComp);
         mScene->AllActors.push_back(skyboxActor);
      }

      TextureAtlasFactory::GetInstance()->AllocateAtlasSpace();
   }
}
