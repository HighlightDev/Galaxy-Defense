#include "SimpleLevel.h"

#include "Core/GameCore/FirstPersonCamera.h"
#include "Core/GameCore/ThirdPersonCamera.h"

#include "Core/GameCore/Components/PrimitiveComponents/BillboardComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/SkyboxComponent.h"
#include "Core/GameCore/Components/PointLightComponent.h"

#include "Core/GameCore/Components/ComponentData/BillboardComponentData.h"
#include "Core/GameCore/Components/ComponentData/MeshComponentData.h"
#include "Core/GameCore/Components/ComponentData/SkyboxComponentData.h"
#include "Core/GameCore/Components/ComponentData/PointLightComponentData.h"
#include "Core/GameCore/Components/ComponentData/InputComponentData.h"
#include "Core/GameCore/Components/ComponentData/PhysicsComponentData.h"
#include "Core/GameCore/Components/MovementComponent.h"

#include "Core/GameCore/GlobalSettings.h"
#include "Core/IoCore/AsyncLoaderCore/ResourceMap.h"

#include "Core/GraphicsCore/Material/WaterDynamicMaterial.h"
#include "Core/GraphicsCore/Material/SkyboxDynamicMaterial.h"

#include "Core/GameCore/Physics/PhysicsWorld.h"

#include "Core/GameCore/ScriptingCore/LuaExecutors/LuaScriptExecutor_EngineObjectsCreator.h"
#include "Core/GraphicsCore/Material/MaterialParser.h"

#include <glm/vec3.hpp>
#include <LogInterface.h>

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

   void SimpleLevel::PreLevelInit()
   {
      Base::PreLevelInit();

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

   void SimpleLevel::PostLevelInit()
   {
      Base::PostLevelInit();

      ResourceMap::DeleteInstance();
      TextureAtlasFactory::GetInstance()->AllocateAtlasSpace();
   }

   void SimpleLevel::LoadLevel()
   {
      ResourceMap::GetInstance()->WaitUntilResourcesLoad();

      //Camera
      {
         mScene->AddCamera(new ThirdPersonCamera("MainCamera", 50, 20, 20));
      }

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

      struct MData : public ComponentData
      {
         MData(const std::string& name)
            : ComponentData(name)
         {

         }
      };
      
      MData data{ "MoveComp" };
      auto moveComp = std::make_shared<MovementComponent>(data.GameObjectName, "platformMovementComponentAction.lua");
      auto actor = static_cast<Actor*>(mScene->GetGameObjectByName("SmallGround"));
      actor->AddComponent(moveComp);

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

         SkyboxComponentData mData("Skybox component", glm::vec3(140.0f), std::make_shared<SkyboxDynamicMaterial>(dayTex, nullptr));
         std::shared_ptr<Actor> skyboxActor = std::make_shared<Actor>("Skybox Actor",
            std::make_shared<SceneComponent>("SkyboxRootComponent", glm::vec3(0), glm::vec3(0), glm::vec3(1)));
         auto skyboxComp = mScene->CreateComponent_GameThread<ComponentMetaType::Skybox, SkyboxComponent>(mData);
         skyboxActor->AddComponent(skyboxComp);
         mScene->AddActor(skyboxActor);
      }
   }
#undef GET_REL_PATH_TO_FILE
}
