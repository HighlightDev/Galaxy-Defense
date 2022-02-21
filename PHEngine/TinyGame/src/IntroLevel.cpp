#include "IntroLevel.h"
#include "Core/GameCore/ScriptingCore/LuaExecutors/LuaScriptExecutor_EngineObjectsCreator.h"
#include "Core/GameCore/ScriptingCore/EngineObjectCreator.h"
#include "Core/GameCore/SpaceShipPlayerController.h"
#include "Core/GameCore/ThirdPersonCamera.h"
#include "Core/GameCore/Components/ComponentData/InputComponentData.h"
#include "Core/GameCore/Components/InputComponent.h"
#include "Core/GameCore/Components/NoPhysicsMovementComponent.h"
#include "Core/GameCore/Tweener/TweenerParser.h"
#include "Core/GameCore/Tweener/Tweener.h"
#include "Core/GameCore/Tweener/BindingAttachmentBuilder.h"

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

   void IntroLevel::CreateScene()
   {
      const auto &a_spaceship = mScene->GetActor("SpaceshipActor");
      assert(a_spaceship);
      const auto &mainCamera = mScene->GetMainCamera();
      assert(mainCamera);
      const std::shared_ptr<SpaceShipPlayerController> &spaceShipController = std::make_shared<SpaceShipPlayerController>(mainCamera, a_spaceship);
      mScene->AddActorController(spaceShipController);

      if (eCameraType::MAIN_THIRD_PERSON_CAMERA == mainCamera->GetCameraType())
      {
         std::static_pointer_cast<ThirdPersonCamera>(mainCamera)->SetThirdPersonTargetDeferred(a_spaceship->GetGameObjectName());
      }

      InputComponentData d_input = InputComponentData("SpaceshipInputComponent");
      const auto &c_input = mScene->CreateComponent_GameThread<InputComponent, eComponentMetaType::Input>(d_input);
      a_spaceship->AddComponent(c_input);

      MovementComponentData d_movement("NoPhysMoveComponentData", glm::vec3());
      const auto &c_movement = mScene->CreateComponent_GameThread<NoPhysicsMovementComponent,
                                                                  eComponentMetaType::Movement>(d_movement);
      a_spaceship->AddComponent(c_movement);

      TweenerParser tweenerParser;
      const auto &spaceshipTweener = tweenerParser.ParseTweenerDescriptor("spaceshipMove.tween");

      a_spaceship->AttachTweener(spaceshipTweener);
      const auto& rootComponent = a_spaceship->GetRootComponent();

      const auto &binding = spaceshipTweener->GetPropertyBindingByName("b_rotator");
      BindingAttachmentBuilder::SetAttachment(rootComponent.get(), binding.get(), "b_rotator");
   }

   void IntroLevel::PostLevelInit()
   {

      CreateScene();
      /*const auto groundActor = mScene->GetActor("Ground");
      const auto pointLightComponents = mScene->GetActor("MainLightActor")->GetComponentsByType<PointLightComponent>();
      const auto plShadowTexAtlasRequest = pointLightComponents[0]->GetRenderData().ShadowInfo->GetTextureAtlasSpaceRequest();

      const CubemapComponentData cubemapComponentData("CubemapComponent", glm::vec3(10, 2, 10), glm::vec3(), glm::vec3(2),
                                                      FolderManager::GetInstance()->GetShadersPath() + "cubemapRendererVS.glsl", FolderManager::GetInstance()->GetShadersPath() + "cubemapRendererFS.glsl", plShadowTexAtlasRequest);
      const auto cubemapRendererComponent = mScene->CreateComponent_GameThread<CubemapComponent, Game::eComponentMetaType::Cubemap>(cubemapComponentData);
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
