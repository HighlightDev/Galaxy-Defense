#include "IntroLevel.h"
#include "Core/GameCore/ScriptingCore/LuaExecutors/LuaScriptExecutor_EngineObjectsCreator.h"
#include "Core/GameCore/ScriptingCore/EngineObjectCreator.h"
#include "Core/GameCore/ThirdPersonCamera.h"
#include "Core/GameCore/Components/ComponentData/InputComponentData.h"
#include "Core/GameCore/Components/InputComponent.h"
#include "Core/GameCore/Components/NoPhysicsMovementComponent.h"
#include "Core/GameCore/Tweener/TweenerParser.h"
#include "Core/GameCore/Tweener/Tweener.h"
#include "Core/GameCore/Tweener/BindingAttachmentBuilder.h"
#include "Core/IoCore/DisplayDeviceDataProvider.h"
#include "Core/GameCore/Event/EventDispatcher.h"
#include "Core/GameCore/Components/PhysicsComponents/GhostPhysicsComponent.h"
#include "Core/GameCore/Components/ComponentData/PhysicsComponentData.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/GhostController.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/PhySphereShape.h"

#include "Implementation/SpaceSceneCamera.h"
#include "Implementation/SpaceShipPlayerController.h"
#include "Implementation/Events/MainPlayerActionEvent.h"

#include <glm/vec4.hpp>
#include <glm/vec3.hpp>

using namespace Graphics;
using namespace EnginePhysics;
using namespace IO;

namespace Game
{

   IntroLevel::IntroLevel(InterThreadCommunicationMgr &threadMgr)
       : Level(threadMgr), mSceneController(std::make_shared<SceneController>(mScene))
   {
      Event::EventDispatcher::GetInstance()->RegisterEventByType<Event::MainPlayerActionEvent>();
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
      mSceneController->PreInit();
   }

   void IntroLevel::CreateScene()
   {
      auto spaceCamera = std::make_shared<SpaceSceneCamera>("SpaceShipCamera",
                                                            eCameraType::MAIN_FIRST_PERSON_CAMERA,
                                                            mScene,
                                                            ViewPortInfo(glm::ivec4(0, 0,
                                                                                    DisplayDeviceDataProvider::GetInstance()->GetWindowWidth(),
                                                                                    DisplayDeviceDataProvider::GetInstance()->GetWindowHeight())),
                                                            38.88f, -2.72f, glm::vec3(5.0f, 45.0f, -40.0f));
      mScene->RegisterMainCamera(spaceCamera);

      const auto &a_spaceship = mScene->GetActor("SpaceshipActor");
      assert(a_spaceship);

      InputComponentData d_input = InputComponentData("SpaceshipInputComponent");
      const auto &c_input = mScene->CreateComponent_GameThread<InputComponent, eComponentMetaType::Input>(d_input);
      a_spaceship->AddComponent(c_input);

      MovementComponentData d_movement("NoPhysMoveComponentData", glm::vec3());
      const auto &c_movement = mScene->CreateComponent_GameThread<NoPhysicsMovementComponent,
                                                                  eComponentMetaType::Movement>(d_movement);

      c_movement->SetSpeed(0.02f);
      a_spaceship->AddComponent(c_movement);

      /*GhostController* ghostController = new GhostController(mScene->GetPhysicsWorld(), new PhySphereShape(5.0f), 0.0f);
      mScene->GetPhysicsWorld()->AddPhysDescriptor(ghostController);
      PhysicsComponentData physData("c_spaceShipPhysicsComponent", ghostController);
      const auto& c_ghostPhysics = mScene->CreateComponent_GameThread<GhostPhysicsComponent, eComponentMetaType::Physics>(physData);
      a_spaceship->AddComponent(c_ghostPhysics);*/

      const auto &mainCamera = mScene->GetMainCamera();
      assert(mainCamera);
      const std::shared_ptr<SpaceShipPlayerController> &spaceShipController = std::make_shared<SpaceShipPlayerController>(mainCamera, a_spaceship);
      mScene->AddActorController(spaceShipController);

      if (eCameraType::MAIN_THIRD_PERSON_CAMERA == mainCamera->GetCameraType())
      {
         std::static_pointer_cast<ThirdPersonCamera>(mainCamera)->SetThirdPersonTargetDeferred(a_spaceship->GetGameObjectName());
      }

      TweenerParser tweenerParser;
      const auto &spaceshipTweener = tweenerParser.ParseTweenerDescriptor("spaceshipMove.tween");

      a_spaceship->AttachTweener(spaceshipTweener);
      const auto &rootComponent = a_spaceship->GetRootComponent();

      const auto &binding = spaceshipTweener->GetPropertyBindingByName("b_rotator");
      BindingAttachmentBuilder::SetAttachment(rootComponent.get(), binding.get(), "b_rotator");

      mScene->AddExternalTickableObject(mSceneController);
      mSceneController->SetPlayerShipActor(a_spaceship);
      mSceneController->SetPlayeActorController(spaceShipController);
   }

   void IntroLevel::PostLevelInit()
   {
      CreateScene();
      mSceneController->PostInit();

      /*const auto groundActor = mScene->GetActor("Ground");
      const auto pointLightComponents = mScene->GetActor("MainLightActor")->GetComponentsByType<PointLightComponent>();
      const auto plShadowTexAtlasRequest = pointLightComponents[0]->GetRenderData().ShadowInfo->GetTextureAtlasSpaceRequest();

      const CubemapComponentData cubemapComponentData("CubemapComponent", glm::vec3(10, 2, 10), glm::vec3(), glm::vec3(2),
                                                      FolderManager::GetInstance()->GetShadersPath() + "cubemapRendererVS.glsl", FolderManager::GetInstance()->GetShadersPath() + "cubemapRendererFS.glsl", plShadowTexAtlasRequest);
      const auto cubemapRendererComponent = mScene->CreateComponent_GameThread<CubemapComponent, EngineCore::eComponentMetaType::Cubemap>(cubemapComponentData);
      groundActor->AddComponent(cubemapRendererComponent);*/
      Base::PostLevelInit();
   }

   void IntroLevel::PostPlayLevelFinished()
   {
      Base::PostPlayLevelFinished();
      mSceneController->PostPlayLevelFinished();
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
