#include "PrologueLevel.h"
#include "Core/GameCore/ScriptingCore/LuaScriptExecutors/LuaEngineScriptExecutor.h"
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
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/CollisionSphereShape.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"

#include "Core/GameCore/Components/ComponentCreators/InputComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/MovementComponentCreator.h"
#include "Core/GameCore/Components/ComponentData/MeshComponentData.h"

#include "Core/GraphicsCore/Material/MaterialParser.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialPropertySetter.h"

#include "Implementation/SpaceSceneCamera.h"
#include "Implementation/Controllers/GameFlowController.h"
#include "Implementation/Controllers/CombatController.h"
#include "Implementation/Events/MainPlayerActionEvent.h"
#include "Implementation/Events/RayCollisionEvent.h"
#include "Implementation/Events/SphereContactCollisionEvent.h"
#include "Implementation/Events/MainPlayerStatusChangedEvent.h"
#include "Implementation/Events/ChangeGameModeEvent.h"

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

   PrologueLevel::PrologueLevel()
       : LevelBase("FirstLevel")
   {
      Event::GameThreadEventDispatcher::GetInstance()->RegisterEventsByType<Event::MainPlayerActionEvent, Event::RayCollisionEvent, Event::SphereContactCollisionEvent, Event::MainPlayerStatusChangedEvent, Event::ChangeGameModeEvent>();
      Event::LuaThreadEventDispatcher::GetInstance()->RegisterEventsByType<Event::LuaMainPlayerStatusChangedEvent>();
   }

   PrologueLevel::~PrologueLevel()
   {
   }

   void PrologueLevel::RunLuaBuildLevelScript()
   {
      const auto sceneSp = mSceneWp.lock();
      assert(sceneSp);
      LuaEngineScriptExecutor mLuaLevelBuilder = LuaEngineScriptExecutor("galaxyDefenseIntroLvl.lua");
      mLuaLevelBuilder.SetScene(sceneSp);
      mLuaLevelBuilder.SetLuaScriptProcessor(sceneSp->GetInterThreadCommunicationManager().GetLuaScriptProcessor());
      mLuaLevelBuilder.RegisterCallbacks();
      mLuaLevelBuilder.RunScript();
      mLuaLevelBuilder.StopScript();
   }

   void PrologueLevel::PreLevelInit()
   {
      const auto sceneSp = mSceneWp.lock();
      assert(sceneSp);
      Base::PreLevelInit();
      mCombatController = std::make_shared<CombatController>(sceneSp);
      mUiController = std::make_unique<UiController>(sceneSp);
      mCombatController->OnPreLevelInit();
      mUiController->OnPreLevelInit();
   }

   void PrologueLevel::CreateScene()
   {
      const auto sceneSp = mSceneWp.lock();
      assert(sceneSp);

      const auto &a_sceneCenterActorDummy = sceneSp->GetActorByName("SceneCenterActorDummy");
      assert(a_sceneCenterActorDummy);
      auto spaceCamera = std::make_shared<ThirdPersonCamera>("LevelMainCamera",
                                                             eCameraType::MAIN_THIRD_PERSON_CAMERA,
                                                             sceneSp,
                                                             ViewPortInfo(0,
                                                                          0,
                                                                          DisplayDeviceDataProvider::GetInstance()->GetWindowWidth(),
                                                                          DisplayDeviceDataProvider::GetInstance()->GetWindowHeight()),
                                                             38.88f,
                                                             -2.72f,
                                                             150.0f);

      spaceCamera->SetMaxDistanceFromTargetToCamera(150.0f);
      spaceCamera->SetMinDistanceFromTargetToCamera(20.0f);
      spaceCamera->SetDistanceFromTargetToCamera(150.0f);
      sceneSp->RegisterMainCamera(spaceCamera);
      spaceCamera->SetThirdPersonTarget(a_sceneCenterActorDummy);

      const auto &a_skybox = sceneSp->GetActorByName("SkyboxActor");
      assert(a_skybox);

      MaterialParser materialParser;
      const std::shared_ptr<IMaterial> &spaceStars_material = materialParser.ParseMaterialDescriptor("SpaceStarsMaterial.m");
      sceneSp->RegisterMaterialInstance(spaceStars_material);

      MaterialPropertySetter::SetMaterialPropertyValue(spaceStars_material, sceneSp, "GT_DeltaSec", "gt_timeSec");
      MaterialPropertySetter::SetMaterialPropertyValue(spaceStars_material, sceneSp, "ScreenResolution", "screenResolution");

      auto billboardComponentCreator = std::make_shared<BillboardComponentCreator<FullscreenBillboardComponent>>();
      const auto backgroundBillboardComponentData = std::make_shared<BillboardComponentData>("c_spaceBackgroundBillboard", 1.0f, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f), spaceStars_material);
      const auto &billboardComponent = std::static_pointer_cast<FullscreenBillboardComponent>(sceneSp->CreateComponent_GameThread(billboardComponentCreator, backgroundBillboardComponentData));
      billboardComponent->SetSortOrderValue(-100000);
      a_skybox->AddComponent(billboardComponent);

      const auto &a_station = sceneSp->GetActorByName("SpaceshipActor");
      const auto &gameFlowController = std::make_shared<GameFlowController>(spaceCamera, a_station->GetRootComponent());
      sceneSp->AddActorController(gameFlowController);

      mCombatController->OnLevelInit();
      mUiController->OnLevelInit();
   }

   void PrologueLevel::PostLevelInit()
   {
      mCombatController->OnPostLevelInit();
      mUiController->OnPostLevelInit();
      Base::PostLevelInit();
   }

   void PrologueLevel::PostPlayLevelFinished()
   {
      Base::PostPlayLevelFinished();
      mCombatController->PostPlayLevelFinished();
      mUiController->PostPlayLevelFinished();
   }

   void PrologueLevel::InitLevel()
   {
      Base::InitLevel();
#if 0
      DeserializeLevel("test_serialize.xml");
#else
      RunLuaBuildLevelScript();
#endif
      CreateScene();
   }

   void PrologueLevel::UnloadLevel()
   {
      if (mUiController)
      {
         mUiController->CleanUp();
         mUiController.reset();
      }

      if (mCombatController)
      {
         mCombatController->CleanUp();
         mCombatController.reset();
      }
   }

   void PrologueLevel::Tick(const float deltaTime)
   {
      if (mUiController)
      {
         mUiController->Tick(deltaTime);
      }

      if (mCombatController)
      {
         mCombatController->Tick(deltaTime);
      }
   }

   void PrologueLevel::UnpausableTick(const float deltaTime)
   {
      if (mUiController)
      {
         mUiController->UnpausableTick(deltaTime);
      }

      if (mCombatController)
      {
         mCombatController->UnpausableTick(deltaTime);
      }
   }
}
