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

#include <glm/vec3.hpp>

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
      // Dir light
      {
         auto directionalLightTextureAtlasRequest = TextureAtlasFactory::GetInstance()->AddTextureAtlasRequest(glm::ivec2(512, 512));
         ProjectedShadowInfo* dirLightInfo = new ProjectedDirShadowInfo(directionalLightTextureAtlasRequest);

         DirectionalLightComponentData mData(glm::vec3(0), glm::vec3(0.5f, -0.5f, 0), glm::vec3(0.2f, 0.2f, 0.2f),
            glm::vec3(0.68f, 0.5f, 0.5f), glm::vec3(0.7f, 0.7f, 0.7f), nullptr);
         std::shared_ptr<Actor> dirLightActor = std::make_shared<Actor>("Main light", std::make_shared<SceneComponent>());
         mScene->CreateAndAddComponent_GameThread<DirectionalLightComponent>(mData, dirLightActor);
         mScene->AllActors.push_back(dirLightActor);
      }

      // Water
      {
         auto normalTex = TexturePool::GetInstance()->GetOrAllocateResource(folderManager->GetNormalMapPath() + "water_normal.png");
         auto distortionTex = TexturePool::GetInstance()->GetOrAllocateResource(folderManager->GetDistortionTexturePath() + "water_dudv.png");
         WaterPlaneComponentData mData(glm::vec3(0), glm::vec3(0), glm::vec3(20), std::make_shared<WaterDynamicMaterial>(normalTex, distortionTex));
         std::shared_ptr<Actor> waterActor = std::make_shared<Actor>("Water", std::make_shared<SceneComponent>(std::move(glm::vec3(0)), std::move(glm::vec3(0)), std::move(glm::vec3(1))));
         mScene->CreateAndAddComponent_GameThread<WaterPlaneComponent>(mData, waterActor);
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
         auto albedoTex = TexturePool::GetInstance()->GetOrAllocateResource(folderManager->GetAlbedoTexturePath() + "city_house_2_Col.png");
         auto normalMapTex = TexturePool::GetInstance()->GetOrAllocateResource(folderManager->GetNormalMapPath() + "city_house_2_Nor.png");
         auto specualrMapTex = TexturePool::GetInstance()->GetOrAllocateResource(folderManager->GetSpecularMapPath() + "city_house_2_Spec.png");

         float x_offset = 0, y_offset = 0.0f;

         for (size_t i = 0; i < 1; ++i)
         {
            StaticMeshComponentData mData(folderManager->GetModelPath() + "City_House_2_BI.obj", glm::vec3(0), glm::vec3(), glm::vec3(0.5f),
               std::make_shared<PBRMaterial>(albedoTex, normalMapTex, specualrMapTex, nullptr, nullptr));

            std::shared_ptr<Actor> houseActor = std::make_shared<Actor>("House Actor", std::make_shared<SceneComponent>(std::move(glm::vec3(10)), std::move(glm::vec3(0)), std::move(glm::vec3(1))));
            mScene->CreateAndAddComponent_GameThread<StaticMeshComponent>(mData, houseActor);
            mScene->AllActors.push_back(houseActor);
         }

      }

      // SKELETAL MESH
      {

         auto albedoTex = TexturePool::GetInstance()->GetOrAllocateResource(folderManager->GetAlbedoTexturePath() + "diffuse.png");
         auto normalMapTex = TexturePool::GetInstance()->GetOrAllocateResource(folderManager->GetNormalMapPath() + "dummy_nm.png");

         SkeletalMeshComponentData mData(folderManager->GetModelPath() + "model.dae", glm::vec3(0, 0, 0), glm::vec3(270, 0, 0), glm::vec3(0.5f),
            std::make_shared<PBRMaterial>(albedoTex, normalMapTex, nullptr, nullptr, nullptr));

         std::shared_ptr<Actor> skeletActor = std::make_shared<Actor>("Buddy", std::make_shared<SceneComponent>(std::move(glm::vec3(10)), std::move(glm::vec3(0)), std::move(glm::vec3(1))));
         mScene->CreateAndAddComponent_GameThread<SkeletalMeshComponent>(mData, skeletActor);

         InputComponentData inputComponentData;
         mScene->CreateAndAddComponent_GameThread<InputComponent>(inputComponentData, skeletActor);
         MovementComponentData movementComponentData(glm::vec3(0), GetCamera());
         mScene->CreateAndAddComponent_GameThread<MovementComponent>(movementComponentData, skeletActor);

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
         mScene->CreateAndAddComponent_GameThread<SkyboxComponent>(mData, skyboxActor);
         mScene->AllActors.push_back(skyboxActor);
      }

      TextureAtlasFactory::GetInstance()->AllocateAtlasSpace();
   }
}
