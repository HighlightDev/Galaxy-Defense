#pragma once

#include "Core/GameCore/Actor.h"
#include "Core/GameCore/ActorController.h"
#include "Core/GameCore/FirstPersonCamera.h"

using namespace Event;
using namespace EngineCore;

namespace Game
{

   class SpaceShipPlayerController
      : public ActorController
   {

      using Base = Actor;

      std::shared_ptr<FirstPersonCamera> m_camera;

      std::string mCurrentState;

   public:

      SpaceShipPlayerController(const std::shared_ptr<ACamera>& playerCamera, const std::shared_ptr<Actor>& actor);

      virtual ~SpaceShipPlayerController();

      virtual void Tick(float deltaTime) override;
      
      virtual void InitActorController() override;
   };

}

