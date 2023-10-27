#pragma once

#include "Core/GameCore/ActorController.h"
#include "Core/GameCore/BoundingBox3D.h"
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
}

namespace Game
{
   class CombatController;

   class GameFlowController
       : public ActorController,
         public ChangeGameModeEvent
   {

      BoundingBox3D mLevelBounds;

      std::unique_ptr<::EngineCore::InputComponent> mInputComponent;

      eGameModeType mCurrentGameModeType{eGameModeType::IDLE};

      std::weak_ptr<ThirdPersonCamera> mMainSceneCamera;

      std::weak_ptr<CombatController> mCombatControllerWp;

      glm::mat4 mProjectionMatrix;

// TEMP 
      std::shared_ptr<SceneComponent> m_tempActorRootComponent;

   public:
      GameFlowController(const std::weak_ptr<ThirdPersonCamera> &mainSceneCamera, std::shared_ptr<SceneComponent> tempActorRootComponent);

      ~GameFlowController() override;

      void Tick(const float deltaTime) override;

      void Initialize() override;

      void ProcessEvent(const typename ChangeGameModeEvent::EventData_t &data) override;

      void SetLevelBounds(const BoundingBox3D &mLevelBounds);

   private:
      void PrepareForSpaceStationPlacementMode();
   };
}
