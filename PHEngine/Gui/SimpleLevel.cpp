#include "SimpleLevel.h"

SimpleLevel::SimpleLevel(InterThreadCommunicationMgr& interThreadCommunicationMgr)
   : Level(interThreadCommunicationMgr)
{

}

void SimpleLevel::LoadLevel()
{
//   const auto& folderManager = Common::FolderManager::GetInstance();
//
//#if 1
//   // PointLight
//   {
//      const size_t total_objects = 50;
//
//      float x_offset = 0, y_offset = 0.0f;
//
//      for (size_t i = 0; i < 4; ++i)
//      {
//         x_offset = i / 5;
//         y_offset = i % 5;
//         // 0
//         std::shared_ptr<Actor> pointLightActor = std::make_shared<Actor>("Point Light Actor", std::make_shared<SceneComponent>(glm::vec3(0, y_offset * 4.0f, -5), glm::vec3(), glm::vec3(1)));
//
//         auto pointLightTextureAtlasRequest = TextureAtlasFactory::GetInstance()->AddTextureCubeAtlasRequest(glm::ivec2(512, 512));
//         //ProjectedPointShadowInfo* pointLightInfo = new ProjectedPointShadowInfo(pointLightTextureAtlasRequest);
//
//         PointLightComponentData mData(glm::vec3(0), glm::vec3(), glm::vec3(0.005f, 0, 0), 3000, glm::vec3(0.2f, 0.2f, 0.2f),
//            glm::vec3(0.68f, 0.5f, 0.5f), glm::vec3(0.7f, 0.7f, 0.7f), nullptr);
//         mScene->CreateAndAddComponent_GameThread<PointLightComponent>(mData, pointLightActor);
//
//         std::shared_ptr<Actor> debugVisualLigthActor = std::make_shared<Actor>("Debug Light Actor", std::make_shared<SceneComponent>(glm::vec3(0, 0, 0), glm::vec3(), glm::vec3(1)));
//
//         BillboardComponentData mBillboardData(glm::vec3(0, 5, 0), glm::vec3(), glm::vec3(1),
//            folderManager->GetShadersPath() + "lampVS.glsl", folderManager->GetShadersPath() + "lampFS.glsl", folderManager->GetShadersPath() + "lampGS.glsl",
//            folderManager->GetEditorTexturePath() + "lamp.png");
//         mScene->CreateAndAddComponent_GameThread<BillboardComponent>(mBillboardData, debugVisualLigthActor);
//
//         pointLightActor->AttachActor(debugVisualLigthActor);
//
//
//         mScene->AllActors.push_back(pointLightActor);
//      }
//
//      // Test for PBR
//      {
//         auto albedoTex = TexturePool::GetInstance()->GetOrAllocateResource(folderManager->GetAlbedoTexturePath() + "city_house_2_Col.png");
//         auto normalMapTex = TexturePool::GetInstance()->GetOrAllocateResource(folderManager->GetNormalMapPath() + "city_house_2_Nor.png");
//         auto specualrMapTex = TexturePool::GetInstance()->GetOrAllocateResource(folderManager->GetSpecularMapPath() + "city_house_2_Spec.png");
//
//         float x_offset = 0, y_offset = 0.0f;
//
//         for (size_t i = 0; i < total_objects; ++i)
//         {
//            x_offset = i / 5;
//            y_offset = i % 5;
//            StaticMeshComponentData mData(folderManager->GetModelPath() + "sphere.obj", glm::vec3(x_offset * 2.5f, y_offset * 2.5f, 0), glm::vec3(), glm::vec3(0.5f),
//               std::make_shared<PBRMaterial>(albedoTex, nullptr, nullptr, nullptr, nullptr));
//
//            std::shared_ptr<Actor> houseActor = std::make_shared<Actor>("House Actor", std::make_shared<SceneComponent>(std::move(glm::vec3(10)), std::move(glm::vec3(0)), std::move(glm::vec3(1))));
//            mScene->CreateAndAddComponent_GameThread<StaticMeshComponent>(mData, houseActor);
//            mScene->AllActors.push_back(houseActor);
//         }
//
//      }
//#if DEBUG
//#if 0
//      std::shared_ptr<Actor> debugVisualLigthActor = std::make_shared<Actor>(std::make_shared<SceneComponent>(glm::vec3(0, 0, 0), glm::vec3(), glm::vec3(1)));
//
//      BillboardComponentData mBillboardData(glm::vec3(0, 5, 0), glm::vec3(), glm::vec3(1),
//         folderManager->GetShadersPath() + "lampVS.glsl", folderManager->GetShadersPath() + "lampFS.glsl", folderManager->GetShadersPath() + "lampGS.glsl",
//         folderManager->GetEditorTexturePath() + "lamp.png");
//      CreateAndAddComponent_GameThread<BillboardComponent>(mBillboardData, debugVisualLigthActor);
//
//      CubemapComponentData mCubemapData(glm::vec3(), glm::vec3(), glm::vec3(1), folderManager->GetShadersPath() + "tSkyboxVS.glsl", folderManager->GetShadersPath() + "cubemapRendererFS.glsl", pointLightTextureAtlasRequest);
//      CreateAndAddComponent_GameThread<CubemapComponent>(mCubemapData, debugVisualLigthActor);
//      pointLightActor->AttachActor(debugVisualLigthActor);
//#endif
//#endif
//   }
//#endif
//
//   // DirectionalLight
//   {
//      // 0
//      /*auto directionalLightTextureAtlasRequest = TextureAtlasFactory::GetInstance()->AddTextureAtlasRequest(glm::ivec2(512, 512));
//      ProjectedDirShadowInfo* dirLightInfo = new ProjectedDirShadowInfo(directionalLightTextureAtlasRequest);
//
//      DirectionalLightComponentData mData(glm::vec3(0),
//         glm::vec3(1, 0, 0),
//         glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.68f, 0.5f, 0.5f), glm::vec3(0.7f, 0.7f, 0.7f), dirLightInfo);
//      std::shared_ptr<Actor> dirLightActor = std::make_shared<Actor>(std::make_shared<SceneComponent>());
//      CreateAndAddComponent_GameThread<DirectionalLightComponent>(mData, dirLightActor);
//      AllActors.push_back(dirLightActor);*/
//
//      //// 1
//      //auto directionalLightTextureAtlasRequest1 = TextureAtlasFactory::GetInstance()->AddTextureAtlasRequest(glm::ivec2(512, 512));
//      //ProjectedDirShadowInfo* dirLightInfo1 = new ProjectedDirShadowInfo(directionalLightTextureAtlasRequest1);
//
//      //DirectionalLightComponentData mData1(glm::vec3(0),
//      //   glm::vec3(-1, 0, 0),
//      //   glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.68f, 0.5f, 0.5f), glm::vec3(0.7f, 0.7f, 0.7f), dirLightInfo1);
//      //std::shared_ptr<Actor> dirLightActor1 = std::make_shared<Actor>(std::make_shared<SceneComponent>());
//      //CreateAndAddComponent_GameThread<DirectionalLightComponent>(mData1, dirLightActor1);
//      //AllActors.push_back(dirLightActor1);
//
//      //// 2
//      //auto directionalLightTextureAtlasRequest2 = TextureAtlasFactory::GetInstance()->AddTextureAtlasRequest(glm::ivec2(512, 512));
//      //ProjectedDirShadowInfo* dirLightInfo2 = new ProjectedDirShadowInfo(directionalLightTextureAtlasRequest2);
//
//      //DirectionalLightComponentData mData2(glm::vec3(0),
//      //   glm::vec3(0, 0, 1),
//      //   glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.68f, 0.5f, 0.5f), glm::vec3(0.7f, 0.7f, 0.7f), dirLightInfo2);
//      //std::shared_ptr<Actor> dirLightActor2 = std::make_shared<Actor>(std::make_shared<SceneComponent>());
//      //CreateAndAddComponent_GameThread<DirectionalLightComponent>(mData2, dirLightActor2);
//      //AllActors.push_back(dirLightActor2);
//
//      //// 3
//      //auto directionalLightTextureAtlasRequest3 = TextureAtlasFactory::GetInstance()->AddTextureAtlasRequest(glm::ivec2(512, 512));
//      //ProjectedDirShadowInfo* dirLightInfo3 = new ProjectedDirShadowInfo(directionalLightTextureAtlasRequest3);
//
//      //DirectionalLightComponentData mData3(glm::vec3(0), glm::vec3(0, 0, -1),
//      //   glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.68f, 0.5f, 0.5f), glm::vec3(0.7f, 0.7f, 0.7f), dirLightInfo3);
//      //std::shared_ptr<Actor> dirLightActor3 = std::make_shared<Actor>(std::make_shared<SceneComponent>());
//      //CreateAndAddComponent_GameThread<DirectionalLightComponent>(mData3, dirLightActor3);
//      //AllActors.push_back(dirLightActor3);
//   }
//
//   //// HOUSE
//   //{
//   //   auto albedoTex = TexturePool::GetInstance()->GetOrAllocateResource(folderManager->GetAlbedoTexturePath() + "city_house_2_Col.png");
//   //   auto normalMapTex = TexturePool::GetInstance()->GetOrAllocateResource(folderManager->GetNormalMapPath() + "city_house_2_Nor.png");
//   //   auto specualrMapTex = TexturePool::GetInstance()->GetOrAllocateResource(folderManager->GetSpecularMapPath() + "city_house_2_Spec.png");
//
//   //   StaticMeshComponentData mData(folderManager->GetModelPath() + "City_House_2_BI.obj", glm::vec3(0), glm::vec3(), glm::vec3(5)
//   //      , std::make_shared<PBRMaterial>(albedoTex, normalMapTex, specualrMapTex, nullptr, nullptr));
//
//   //   std::shared_ptr<Actor> houseActor = std::make_shared<Actor>(std::make_shared<SceneComponent>(std::move(glm::vec3(10)), std::move(glm::vec3(0)), std::move(glm::vec3(1))));
//   //   CreateAndAddComponent_GameThread<StaticMeshComponent>(mData, houseActor);
//   //   AllActors.push_back(houseActor);
//   //}
//
//   // SKELETAL MESH
//   {
//
//      auto albedoTex = TexturePool::GetInstance()->GetOrAllocateResource(folderManager->GetAlbedoTexturePath() + "diffuse.png");
//      auto normalMapTex = TexturePool::GetInstance()->GetOrAllocateResource(folderManager->GetNormalMapPath() + "dummy_nm.png");
//
//      SkeletalMeshComponentData mData(folderManager->GetModelPath() + "model.dae", glm::vec3(0, 0, 0), glm::vec3(270, 0, 0), glm::vec3(1),
//         folderManager->GetShadersPath() + "skeletalMeshVS.glsl",
//         folderManager->GetShadersPath() + "skeletalMeshFS.glsl",
//         std::make_shared<PBRMaterial>(albedoTex, normalMapTex, nullptr, nullptr, nullptr));
//      std::shared_ptr<Actor> skeletActor = std::make_shared<Actor>("Buddy", std::make_shared<SceneComponent>(std::move(glm::vec3(10)), std::move(glm::vec3(0)), std::move(glm::vec3(1))));
//      mScene->CreateAndAddComponent_GameThread<SkeletalMeshComponent>(mData, skeletActor);
//
//      InputComponentData inputComponentData;
//      mScene->CreateAndAddComponent_GameThread<InputComponent>(inputComponentData, skeletActor);
//      MovementComponentData movementComponentData(glm::vec3(0), GetCamera());
//      mScene->CreateAndAddComponent_GameThread<MovementComponent>(movementComponentData, skeletActor);
//
//      mScene->m_playerController.SetPlayerActor(skeletActor);
//
//      mScene->AllActors.push_back(skeletActor);
//
//      ICamera* camera = GetCamera();
//
//      if (camera->GetCameraType() == ICamera::CameraType::THIRD_PERSON)
//      {
//         static_cast<ThirdPersonCamera*>(camera)->SetThirdPersonTarget(skeletActor);
//      }
//   }
//
//   // SKYBOX
//   {
//      StringStreamWrapper::ToString(
//         folderManager->GetCubemapTexturePath(), "Day/", "right.png", ",",
//         folderManager->GetCubemapTexturePath(), "Day/", "left.png", ",",
//         folderManager->GetCubemapTexturePath(), "Day/", "top.png", ",",
//         folderManager->GetCubemapTexturePath(), "Day/", "bottom.png", ",",
//         folderManager->GetCubemapTexturePath(), "Day/", "back.png", ",",
//         folderManager->GetCubemapTexturePath(), "Day/", "front.png");
//      auto dTexPath = StringStreamWrapper::FlushString();
//
//      SkyboxComponentData mData(glm::vec3(140.0f), 5.0f, std::move(folderManager->GetShadersPath() + "tSkyboxVS.glsl"),
//         std::move(folderManager->GetShadersPath() + "tSkyboxFS.glsl"), std::move(dTexPath));
//      std::shared_ptr<Actor> skyboxActor = std::make_shared<Actor>("Skybox Actor", std::make_shared<SceneComponent>(std::move(glm::vec3(0)), std::move(glm::vec3(0)), std::move(glm::vec3(1))));
//      mScene->CreateAndAddComponent_GameThread<SkyboxComponent>(mData, skyboxActor);
//      mScene->AllActors.push_back(skyboxActor);
//   }
//
//   TextureAtlasFactory::GetInstance()->AllocateAtlasSpace();
}