#pragma once

#include "Core/GameCore/BoundingBox3D.h"
#include "Core/GameCore/ITickable.h"
#include "Implementation/Controllers/ILevelController.h"
#include "Implementation/Events/ChangeGameModeEvent.h"
#include "Implementation/GameModeTypeEnum.h"

#include <glm/mat4x4.hpp>

using namespace Event;
using namespace EngineCore;

namespace EngineCore
{
   class InputComponent;
   class ThirdPersonCamera;
   class SceneComponent;
   class Scene;
}

namespace Game
{
   class GameFlowController
       : public ILevelController,
         public ITickable,
         public ChangeGameModeEvent,
         public std::enable_shared_from_this<GameFlowController>
   {
      std::weak_ptr<::EngineCore::Scene> mSceneWp;

      BoundingBox3D mLevelBounds;

      std::unique_ptr<::EngineCore::InputComponent> mInputComponent;

      eGameModeType mCurrentGameModeType{eGameModeType::IDLE};

      std::weak_ptr<ThirdPersonCamera> mMainSceneCamera;

      glm::mat4 mProjectionMatrix;

      // TEMP
      std::shared_ptr<SceneComponent> m_tempActorRootComponent;

   public:
      GameFlowController(const std::weak_ptr<::EngineCore::Scene>& sceneWp);

      ~GameFlowController() override;

      void Tick(const float deltaTime) override;

      void UnpausableTick(const float deltaTime) override;

      void OnPreLevelInit() override;

      void OnLevelInit() override;

      void OnPostLevelInit() override;

      void PostPlayLevelFinished() override;

      void CleanUp() override;

      void ProcessEvent(const typename ChangeGameModeEvent::EventData_t &data) override;

      void Initialize();

      void SetLevelBounds(const BoundingBox3D &mLevelBounds);

      void SetTempRootComponent(const std::shared_ptr<SceneComponent>& tempActorRootComponent);

   private:
      void PrepareForSpaceStationPlacementMode();
   };
}
