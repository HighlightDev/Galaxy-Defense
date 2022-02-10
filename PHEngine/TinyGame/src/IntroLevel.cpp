#include "IntroLevel.h"
#include "Core/GameCore/ScriptingCore/LuaExecutors/LuaScriptExecutor_EngineObjectsCreator.h"
#include "Core/GameCore/ScriptingCore/EngineObjectCreator.h"
#include "Core/GameCore/SpaceShipPlayerController.h"
#include "Core/GameCore/ThirdPersonCamera.h"

using namespace Graphics;
using namespace EnginePhysics;
using namespace IO;

namespace Labyrinth
{

   IntroLevel::IntroLevel(InterThreadCommunicationMgr &threadMgr)
       : Level(threadMgr)
   {
   }

   IntroLevel::~IntroLevel()
   {
   }

   void IntroLevel::RunLuaBuildLevelScript()
   {
      static constexpr const char *lvlName
          // = "createTestLevel.lua";
          = "spaceLvl1.lua";
      LuaScriptExecutor_EngineObjectsCreator mLuaLevelBuilder = LuaScriptExecutor_EngineObjectsCreator(lvlName);
      mLuaLevelBuilder.PostInit(mScene);
      mLuaLevelBuilder.RegisterCallbacks();
      mLuaLevelBuilder.RunScript();
   }

   void IntroLevel::PreLevelInit()
   {
      Base::PreLevelInit();
   }

   void IntroLevel::PostLevelInit()
   {
      const auto &a_spaceship = mScene->GetActor("SpaceshipActor");
      assert(a_spaceship);
      const auto &mainCamera = mScene->GetMainCamera();
      assert(mainCamera);
      std::shared_ptr<SpaceShipPlayerController> spaceShipController = std::make_shared<SpaceShipPlayerController>(mainCamera, a_spaceship);
      mScene->SetPlayerController(spaceShipController);

      if (eCameraType::MAIN_THIRD_PERSON_CAMERA == mainCamera->GetCameraType())
      {
         std::static_pointer_cast<ThirdPersonCamera>(mainCamera)->SetThirdPersonTargetDeferred(a_spaceship->GetGameObjectName());
      }

      /*const auto groundActor = mScene->GetActor("Ground");
      const auto pointLightComponents = mScene->GetActor("MainLightActor")->GetComponentsByType<PointLightComponent>();
      const auto plShadowTexAtlasRequest = pointLightComponents[0]->GetRenderData().ShadowInfo->GetTextureAtlasSpaceRequest();

      const CubemapComponentData cubemapComponentData("CubemapComponent", glm::vec3(10, 2, 10), glm::vec3(), glm::vec3(2),
                                                      FolderManager::GetInstance()->GetShadersPath() + "cubemapRendererVS.glsl", FolderManager::GetInstance()->GetShadersPath() + "cubemapRendererFS.glsl", plShadowTexAtlasRequest);
      const auto cubemapRendererComponent = mScene->CreateComponent_GameThread<CubemapComponent, Game::ComponentMetaType::Cubemap>(cubemapComponentData);
      groundActor->AddComponent(cubemapRendererComponent);*/
      Base::PostLevelInit();
   }

   void IntroLevel::LoadLevel()
   {
      // ResourceMap::GetInstance()->WaitUntilResourcesLoad();

#if 0
      DeserializeLevel("test_serialize.xml");
#else
      RunLuaBuildLevelScript();
#endif
   }
#undef GET_REL_PATH_TO_FILE
}
