#pragma once

#include "ActorController.h"
#include "Core/GameCore/ThirdPersonCamera.h"

using namespace Event;

namespace Game
{
   class ACamera;
   
   class SpaceShipPlayerController
      : public ActorController
   {

      using Base = Actor;

      std::shared_ptr<ThirdPersonCamera> m_camera;

   public:

      SpaceShipPlayerController(std::shared_ptr<ACamera> playerCamera, std::shared_ptr<Actor> playerActor);

      virtual ~SpaceShipPlayerController();

      virtual void Tick(float deltaTime) override;

   protected:

      virtual void InitPlayerController() override;
   };

}

