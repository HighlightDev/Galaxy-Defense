#include "SimpleLevel.h"

#include "Core/GameCore/FirstPersonCamera.h"
#include "Core/GameCore/MainThirdPersonCamera.h"
#include "Core/GameCore/GlobalInputController.h"

#include "Core/GameCore/Components/PrimitiveComponents/BillboardComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/SkyboxComponent.h"
#include "Core/GameCore/Components/PointLightComponent.h"
#include "Core/GameCore/Components/PlanarReflectionComponent.h"
#include "Core/GameCore/Components/MovementComponent.h"

#include "Core/GameCore/Components/ComponentData/BillboardComponentData.h"
#include "Core/GameCore/Components/ComponentData/MeshComponentData.h"
#include "Core/GameCore/Components/ComponentData/SkyboxComponentData.h"
#include "Core/GameCore/Components/ComponentData/PointLightComponentData.h"
#include "Core/GameCore/Components/ComponentData/InputComponentData.h"
#include "Core/GameCore/Components/ComponentData/PhysicsComponentData.h"
#include "Core/GameCore/Components/ComponentData/PlanarReflectionComponentData.h"

#include "Core/GameCore/GlobalSettings.h"
#include "Core/IoCore/AsyncLoaderCore/ResourceMap.h"

#include "Core/GraphicsCore/Material/WaterDynamicMaterial.h"
#include "Core/GraphicsCore/Material/SkyboxDynamicMaterial.h"

#include "Core/GameCore/Physics/PhysicsWorld.h"

#include "Core/GameCore/ScriptingCore/LuaExecutors/LuaScriptExecutor_EngineObjectsCreator.h"
#include "Core/GraphicsCore/Material/MaterialParser.h"

#include <glm/vec3.hpp>

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
      LuaScriptExecutor_EngineObjectsCreator mLuaLevelBuilder = LuaScriptExecutor_EngineObjectsCreator("createTestLevel.lua");
      mLuaLevelBuilder.PostInit(mScene);
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
         int32_t windowWidth = GlobalInputController::GetInstance()->GetWindowWidth();
         int32_t windowHeight = GlobalInputController::GetInstance()->GetWindowHeight();

         ViewPortInfo viewPort{ 0 , 0, windowWidth, windowHeight };

         mScene->RegisterMainCamera(std::make_shared<MainThirdPersonCamera>("MainCamera", mScene, viewPort, 50.0f, 20.0f, 20.0f));
      }

      // todo: only test
#if false
      DeserializeLevel("test_serialize.xml");
#else
      RunLuaBuildLevelScript();
#endif

      int32_t windowWidth = GlobalInputController::GetInstance()->GetWindowWidth();
      int32_t windowHeight = GlobalInputController::GetInstance()->GetWindowHeight();

      glm::vec3 normal(0, 1, 0);
      glm::vec3 posOnPlane(0, 20, 0);
      float d = -glm::dot(normal, posOnPlane);
      glm::vec4 plane = glm::vec4(normal, d);

      auto cameraPtr = mScene->GetMainCamera().get();
      PlanarReflectionComponentData data{ "PlanarReflectionComp", glm::vec3(), glm::vec3(), glm::vec3(1), plane, cameraPtr,
      ViewPortInfo(0,0,windowWidth, windowHeight) };
      volatile auto planarReflectionComp =
         std::static_pointer_cast<PlanarReflectionComponent>(mScene->CreateComponent_GameThread<ComponentMetaType::PlanarReflection, PlanarReflectionComponent>(data));


#if false
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
   }
#undef GET_REL_PATH_TO_FILE
}
