#include "IntroLevel.h"
#include "Core/GameCore/ScriptingCore/LuaScriptExecutors/LuaEngineScriptExecutor.h"
#include "Core/GameCore/ScriptingCore/EngineObjectCreator.h"
#include "Core/GameCore/ThirdPersonCamera.h"
#include "Core/GameCore/Components/InputComponent.h"
#include "Core/GameCore/Components/NoPhysicsMovementComponent.h"
#include "Core/GameCore/Tweener/TweenerParser.h"
#include "Core/GameCore/Tweener/Tweener.h"
#include "Core/GameCore/Tweener/BindingAttachmentBuilder.h"
#include "Core/IoCore/DisplayDeviceDataProvider.h"
#include "Core/GameCore/Event/GameThreadEventDispatcher.h"
#include "Core/GameCore/Event/LuaThreadEventDispatcher.h"
#include "Core/GameCore/Components/PhysicsComponents/GhostPhysicsComponent.h"
#include "Core/GameCore/Components/ComponentData/PhysicsComponentData.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/GhostController.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/PhySphereShape.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"

#include "Core/GameCore/Components/ComponentCreators/InputComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/MovementComponentCreator.h"
#include "Core/GameCore/Components/ComponentData/MeshComponentData.h"

#include "Core/GraphicsCore/Material/MaterialParser.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialPropertySetter.h"

#include "Implementation/SpaceSceneCamera.h"
#include "Implementation/Controllers/SpaceShipPlayerController.h"
#include "Implementation/Events/MainPlayerActionEvent.h"
#include "Implementation/Events/RayCollisionEvent.h"
#include "Implementation/Events/SphereContactCollisionEvent.h"
#include "Implementation/Events/MainPlayerStatusChangedEvent.h"

#include "Core/GameCore/Components/ComponentData/BillboardComponentData.h"
#include "Core/GameCore/Components/PrimitiveComponents/FullscreenBillboardComponent.h"
#include "Core/GameCore/Components/ComponentCreators/BillboardComponentCreator.h"

#include <glm/vec4.hpp>
#include <glm/vec3.hpp>

using namespace Graphics;
using namespace EnginePhysics;
using namespace IO;
using namespace EngineCore;
using namespace EngineCore::Scripts;
using namespace Resources;
using namespace Graphics;

namespace Game
{

   IntroLevel::IntroLevel()
       : Level("FirstLevel")
   {
      Event::GameThreadEventDispatcher::GetInstance()->RegisterEventsByType<Event::MainPlayerActionEvent, Event::RayCollisionEvent, Event::SphereContactCollisionEvent, Event::MainPlayerStatusChangedEvent>();
      Event::LuaThreadEventDispatcher::GetInstance()->RegisterEventsByType<Event::LuaMainPlayerStatusChangedEvent>();
   }

   IntroLevel::~IntroLevel()
   {
   }

   void IntroLevel::RunLuaBuildLevelScript()
   {
      const auto sceneSp = mSceneWp.lock();
      assert(sceneSp);
      static constexpr const char *lvlName
          // = "createTestLevel.lua";
          = "spaceLvl1.lua";
      LuaEngineScriptExecutor mLuaLevelBuilder = LuaEngineScriptExecutor(lvlName);
      mLuaLevelBuilder.SetScene(sceneSp);
      mLuaLevelBuilder.SetLuaScriptProcessor(sceneSp->GetInterThreadCommunicationManager().GetLuaScriptProcessor());
      mLuaLevelBuilder.RegisterCallbacks();
      mLuaLevelBuilder.RunScript();
      mLuaLevelBuilder.StopScript();
   }

   void IntroLevel::PreLevelInit()
   {
      const auto sceneSp = mSceneWp.lock();
      assert(sceneSp);
      mSceneController = std::make_shared<SceneController>(sceneSp);
      Base::PreLevelInit();
      mSceneController->OnPreLevelInit();
   }

   void IntroLevel::CreateScene()
   {
      const auto sceneSp = mSceneWp.lock();
      assert(sceneSp);
      const auto displayWidth = DisplayDeviceDataProvider::GetInstance()->GetWindowWidth();
      const auto displayHeight = DisplayDeviceDataProvider::GetInstance()->GetWindowHeight();

      auto spaceCamera = std::make_shared<SpaceSceneCamera>("SpaceShipCamera",
                                                            eCameraType::MAIN_FIRST_PERSON_CAMERA,
                                                            sceneSp,
                                                            ViewPortInfo(0, 0, displayWidth, displayHeight),
                                                            38.88f,
                                                            -2.72f,
                                                            glm::vec3(5.0f, 45.0f, -40.0f));
      sceneSp->RegisterMainCamera(spaceCamera);

      const auto &a_skybox = sceneSp->GetActorByName("SkyboxActor");
      assert(a_skybox);

      MaterialParser materialParser;
      const auto &spaceStars_material = materialParser.ParseMaterialDescriptor("SpaceStarsMaterial.m");
      const auto screenResolution = glm::vec2((float)displayWidth, (float)displayHeight);

      MaterialPropertySetter::SetMaterialPropertyValue(spaceStars_material, sceneSp.get(), "GT_DeltaSec", "gt_timeSec");
      MaterialPropertySetter::SetMaterialPropertyValue(spaceStars_material, "resolution", screenResolution);

      auto billboardComponentCreator = std::make_shared<BillboardComponentCreator<FullscreenBillboardComponent>>();
      BillboardComponentData backgroundBillboardComponentData("c_spaceBackgroundBillboard", 1.0f, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f), spaceStars_material);
      const auto &billboardComponent = std::static_pointer_cast<FullscreenBillboardComponent>(sceneSp->CreateComponent_GameThread(billboardComponentCreator, backgroundBillboardComponentData));
      billboardComponent->SetSortOrderValue(-100000);
      a_skybox->AddComponent(billboardComponent);

      const auto &a_spaceship = sceneSp->GetActorByName("SpaceshipActor");
      assert(a_spaceship);

      ComponentData d_input = ComponentData("SpaceshipInputComponent");
      const auto &inputComponentCreator = std::make_shared<InputComponentCreator<InputComponent>>();
      const auto &c_input = sceneSp->CreateComponent_GameThread(inputComponentCreator, d_input);
      a_spaceship->AddComponent(c_input);

      MovementComponentData d_movement("NoPhysMoveComponentData", glm::vec3());
      const auto &movementComponentCreator = std::make_shared<MovementComponentCreator<NoPhysicsMovementComponent>>();
      const auto &c_movement = std::static_pointer_cast<NoPhysicsMovementComponent>(sceneSp->CreateComponent_GameThread(movementComponentCreator, d_movement));

      c_movement->SetReferenceSpeed(40.0f);
      c_movement->SetCurrentSpeedToReferenceValue();
      a_spaceship->AddComponent(c_movement);

      const auto &mainCamera = sceneSp->GetMainCamera();
      assert(mainCamera);
      const std::shared_ptr<SpaceShipPlayerController> &spaceShipController = std::make_shared<SpaceShipPlayerController>(mainCamera, a_spaceship);
      sceneSp->AddActorController(spaceShipController);

      if (eCameraType::MAIN_THIRD_PERSON_CAMERA == mainCamera->GetCameraType())
      {
         std::static_pointer_cast<ThirdPersonCamera>(mainCamera)->SetThirdPersonTargetDeferred(a_spaceship->GetEngineObjectName());
      }

      TweenerParser tweenerParser;
      const auto &spaceshipTweener = tweenerParser.ParseTweenerDescriptor("spaceshipMove.tween");

      a_spaceship->AttachTweener(spaceshipTweener);
      const auto &rootComponent = a_spaceship->GetRootComponent();

      const auto &binding = spaceshipTweener->GetPropertyBindingByName("b_rotator");
      BindingAttachmentBuilder::SetAttachment(rootComponent.get(), binding.get(), "b_rotator");

      mSceneController->SetPlayerActorController(spaceShipController);

      /*const auto groundActor = sceneSp->GetActorByName("Ground");
      const auto pointLightComponents = sceneSp->GetActorByName("MainLightActor")->GetComponentsByType<PointLightComponent>();
      const auto plShadowTexAtlasRequest = pointLightComponents[0]->GetRenderData().ShadowInfo->GetTextureAtlasSpaceRequest();

      const CubemapComponentData cubemapComponentData("CubemapComponent", glm::vec3(10, 2, 10), glm::vec3(), glm::vec3(2),
                                                      FolderManager::GetInstance()->GetShadersPath() + "cubemapRendererVS.glsl", FolderManager::GetInstance()->GetShadersPath() + "cubemapRendererFS.glsl", plShadowTexAtlasRequest);
      const auto cubemapRendererComponent = sceneSp->CreateComponent_GameThread<CubemapComponent, EngineCore::eComponentMetaType::Cubemap>(cubemapComponentData);
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
#if 0
      DeserializeLevel("test_serialize.xml");
#else
      RunLuaBuildLevelScript();
#endif
      CreateScene();
   }

   void IntroLevel::UnloadLevel()
   {
      if (mSceneController)
      {
         mSceneController->CleanUp();
         mSceneController.reset();
      }
   }

   void IntroLevel::Tick(const float deltaTime)
   {
      if (mSceneController)
      {
         mSceneController->Tick(deltaTime);
      }
   }

   void IntroLevel::UnpausableTick(const float deltaTime)
   {
      if (mSceneController)
      {
         mSceneController->UnpausableTick(deltaTime);
      }
   }
}
