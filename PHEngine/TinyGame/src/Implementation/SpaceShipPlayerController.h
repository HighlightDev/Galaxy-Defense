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

      SpaceShipPlayerController(std::shared_ptr<ACamera> playerCamera, std::shared_ptr<Actor> playerActor);

      virtual ~SpaceShipPlayerController();

      virtual void Tick(float deltaTime) override;

   protected:

      virtual void InitPlayerController() override;
   };

}

