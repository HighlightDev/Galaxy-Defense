#include "IntroLevel.h"
#include "Core/GameCore/ScriptingCore/LuaExecutors/LuaScriptExecutor_EngineObjectsCreator.h"
#include "Core/GameCore/ScriptingCore/EngineObjectCreator.h"
#include "Core/GameCore/ThirdPersonCamera.h"
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
#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"

#include "Core/GameCore/Components/ComponentCreators/InputComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/MovementComponentCreator.h"

#include "Implementation/SpaceSceneCamera.h"
#include "Implementation/Controllers/SpaceShipPlayerController.h"
#include "Implementation/Events/MainPlayerActionEvent.h"

#include <glm/vec4.hpp>
#include <glm/vec3.hpp>

using namespace Graphics;
using namespace EnginePhysics;
using namespace IO;
using namespace EngineCore;
using namespace Resources;
using namespace Graphics;

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
      mSceneController->OnPreLevelInit();
   }

   void IntroLevel::CreateScene()
   {
      const float displayWidth = DisplayDeviceDataProvider::GetInstance()->GetWindowWidth();
      const float displayHeight = DisplayDeviceDataProvider::GetInstance()->GetWindowHeight();

      auto spaceCamera = std::make_shared<SpaceSceneCamera>("SpaceShipCamera",
                                                            eCameraType::MAIN_FIRST_PERSON_CAMERA,
                                                            mScene,
                                                            ViewPortInfo(0, 0, displayWidth, displayHeight),
                                                            38.88f,
                                                            -2.72f,
                                                            glm::vec3(5.0f, 45.0f, -40.0f));
      mScene->RegisterMainCamera(spaceCamera);

      const auto &a_spaceship = mScene->GetActorByName("SpaceshipActor");
      assert(a_spaceship);

      ComponentData d_input = ComponentData("SpaceshipInputComponent");
      const auto &inputComponentCreator = std::make_shared<InputComponentCreator<InputComponent>>();
      const auto &c_input = mScene->CreateComponent_GameThread(inputComponentCreator, d_input);
      a_spaceship->AddComponent(c_input);

      MovementComponentData d_movement("NoPhysMoveComponentData", glm::vec3());
      const auto &movementComponentCreator = std::make_shared<MovementComponentCreator<NoPhysicsMovementComponent>>();
      const auto &c_movement = std::static_pointer_cast<NoPhysicsMovementComponent>(mScene->CreateComponent_GameThread(movementComponentCreator, d_movement));

      c_movement->SetSpeed(20.0f);
      a_spaceship->AddComponent(c_movement);

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
      mSceneController->SetPlayerActorController(spaceShipController);

      TexturePool::GetInstance()->GetOrAllocateResource("arial.png");

      /*const auto groundActor = mScene->GetActorByName("Ground");
      const auto pointLightComponents = mScene->GetActorByName("MainLightActor")->GetComponentsByType<PointLightComponent>();
      const auto plShadowTexAtlasRequest = pointLightComponents[0]->GetRenderData().ShadowInfo->GetTextureAtlasSpaceRequest();

      const CubemapComponentData cubemapComponentData("CubemapComponent", glm::vec3(10, 2, 10), glm::vec3(), glm::vec3(2),
                                                      FolderManager::GetInstance()->GetShadersPath() + "cubemapRendererVS.glsl", FolderManager::GetInstance()->GetShadersPath() + "cubemapRendererFS.glsl", plShadowTexAtlasRequest);
      const auto cubemapRendererComponent = mScene->CreateComponent_GameThread<CubemapComponent, EngineCore::eComponentMetaType::Cubemap>(cubemapComponentData);
      groundActor->AddComponent(cubemapRendererComponent);*/

      mSceneController->OnLevelInit();
   }

   void IntroLevel::PostLevelInit()
   {
      mSceneController->OnPostLevelInit();
      Base::PostLevelInit();
   }

   void IntroLevel::PostPlayLevelFinished()
   {
      Base::PostPlayLevelFinished();
      mSceneController->PostPlayLevelFinished();
   }

   void IntroLevel::InitLevel()
   {
      Base::InitLevel();
      // ResourceMap::GetInstance()->WaitUntilResourcesLoad();
#if 0
      DeserializeLevel("test_serialize.xml");
#else
      RunLuaBuildLevelScript();
#endif
      CreateScene();
   }
#undef GET_REL_PATH_TO_FILE
}
