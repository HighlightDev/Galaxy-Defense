#include "SimpleLevel.h"

#include "Core/GameCore/FirstPersonCamera.h"
#include "Core/GameCore/MainThirdPersonCamera.h"
#include "Core/IoCore/DisplayDeviceDataProvider.h"

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

#include "Core/GameCore/Physics/PhysicsWorld.h"

#include "Core/GameCore/ScriptingCore/LuaExecutors/LuaScriptExecutor_EngineObjectsCreator.h"
#include "Core/GraphicsCore/Material/MaterialParser.h"

#include <glm/vec3.hpp>
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/PhyCompoundShape.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/PhySphereShape.h"
#include "Core/GameCore/ScriptingCore/EngineObjectCreator.h"

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
      folderManager->CreateFilePathMap(folderManager->GetDistortionTexturePath());
      folderManager->CreateFilePathMap(folderManager->GetCubemapTexturePath());
      folderManager->CreateFilePathMap(folderManager->GetMaterialTexturesPath());
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

      ALLOC_RES_ASYNC(GET_REL_PATH_TO_FILE("nightRight.png"));
      ALLOC_RES_ASYNC(GET_REL_PATH_TO_FILE("nightLeft.png"));
      ALLOC_RES_ASYNC(GET_REL_PATH_TO_FILE("nightTop.png"));
      ALLOC_RES_ASYNC(GET_REL_PATH_TO_FILE("nightBottom.png"));
      ALLOC_RES_ASYNC(GET_REL_PATH_TO_FILE("nightBack.png"));
      ALLOC_RES_ASYNC(GET_REL_PATH_TO_FILE("nightFront.png"));
      ALLOC_RES_ASYNC(GET_REL_PATH_TO_FILE("water_dudv.png"));
      ALLOC_RES_ASYNC(GET_REL_PATH_TO_FILE("Brick_Medieval_albedo.png"));
      ALLOC_RES_ASYNC(GET_REL_PATH_TO_FILE("Brick_Medieval_normal.png"));
      ALLOC_RES_ASYNC(GET_REL_PATH_TO_FILE("Brick_Medieval_roughness.png"));
      ALLOC_RES_ASYNC(GET_REL_PATH_TO_FILE("Brick_Medieval_metallic.png"));
      ALLOC_RES_ASYNC(GET_REL_PATH_TO_FILE("dummy_metallic_roughness.png"));
      
      ALLOC_RES_ASYNC(GET_REL_PATH_TO_FILE("playerCube.obj"));
      ALLOC_RES_ASYNC(GET_REL_PATH_TO_FILE("witcher.obj"));
      ALLOC_RES_ASYNC(GET_REL_PATH_TO_FILE("City_House_2_BI.obj"));
      ALLOC_RES_ASYNC(GET_REL_PATH_TO_FILE("model.dae"));
      ALLOC_RES_ASYNC(GET_REL_PATH_TO_FILE("player_walk.fbx"));
      //ALLOC_RES_ASYNC(GET_REL_PATH_TO_FILE("tina.fbx"));

#undef ALLOC_RES_ASYNC
   }

   void SimpleLevel::PostLevelInit()
   {
      Base::PostLevelInit();
   }

   void SimpleLevel::LoadLevel()
   {
      ResourceMap::GetInstance()->WaitUntilResourcesLoad();

      //Camera
      {
         int32_t windowWidth = DisplayDeviceDataProvider::GetInstance()->GetWindowWidth();
         int32_t windowHeight = DisplayDeviceDataProvider::GetInstance()->GetWindowHeight();

         ViewPortInfo viewPort{ 0 , 0, windowWidth, windowHeight };

         mScene->RegisterMainCamera(std::make_shared<MainThirdPersonCamera>("MainCamera", mScene, viewPort, 50.0f, 20.0f, 20.0f, glm::vec3(0, 5, 0)));
      }

#if false
      DeserializeLevel("test_serialize.xml");
#else
      RunLuaBuildLevelScript();
#endif
   }
#undef GET_REL_PATH_TO_FILE
}
