#pragma once

#include "ActorController.h"

using namespace Event;

namespace Game
{
   class ACamera;
   
   class SpaceShipPlayerController
      : public ActorController
   {

      using Base = Actor;

      std::shared_ptr<ACamera> m_camera;

   public:

      SpaceShipPlayerController(std::shared_ptr<ACamera> playerCamera, std::shared_ptr<Actor> playerActor);

      virtual ~SpaceShipPlayerController();

      virtual void Tick(float deltaTime) override;

   protected:

      virtual void InitPlayerController() override;
   };

}

