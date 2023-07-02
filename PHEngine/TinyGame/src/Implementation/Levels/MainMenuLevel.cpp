#include "MainMenuLevel.h"
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

   MainMenuLevel::MainMenuLevel()
       : Level("MainMenuLevel")
       
   {
   }

   MainMenuLevel::~MainMenuLevel()
   {
   }

   void MainMenuLevel::PreLevelInit()
   {
      Base::PreLevelInit();
      const auto sceneSp = mSceneWp.lock();
      assert(sceneSp);
      mUiController = std::make_unique<MainMenuLevelUiController>(sceneSp);
      mUiController->OnPreLevelInit();
   }

   void MainMenuLevel::CreateScene()
   {
      RunLuaBuildLevelScript();
   }

   void MainMenuLevel::RunLuaBuildLevelScript()
   {
      const auto sceneSp = mSceneWp.lock();
      assert(sceneSp);
      static constexpr const char *lvlName
          = "mainMenuLvl.lua";
      LuaEngineScriptExecutor mLuaLevelBuilder = LuaEngineScriptExecutor(lvlName);
      mLuaLevelBuilder.SetScene(sceneSp);
      mLuaLevelBuilder.SetLuaScriptProcessor(sceneSp->GetInterThreadCommunicationManager().GetLuaScriptProcessor());
      mLuaLevelBuilder.RegisterCallbacks();
      mLuaLevelBuilder.RunScript();
      mLuaLevelBuilder.StopScript();
   }

   void MainMenuLevel::PostLevelInit()
   {
      Base::PostLevelInit();
      mUiController->OnPostLevelInit();
   }

   void MainMenuLevel::PostPlayLevelFinished()
   {
      Base::PostPlayLevelFinished();
      mUiController->PostPlayLevelFinished();
   }

   void MainMenuLevel::InitLevel()
   {
      Base::InitLevel();
      CreateScene();
      mUiController->OnLevelInit();
   }

   void MainMenuLevel::UnloadLevel()
   {
      mUiController->CleanUp();
      mUiController.reset();
   }

   void MainMenuLevel::Tick(const float deltaTime)
   {
      if (mUiController)
      {
         mUiController->Tick(deltaTime);
      }
   }

   void MainMenuLevel::UnpausableTick(const float deltaTime)
   {
      if (mUiController)
      {
         mUiController->UnpausableTick(deltaTime);
      }
   }
}
