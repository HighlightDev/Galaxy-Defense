#pragma once

#include "Core/GameCore/Actor.h"
#include "Core/GameCore/ActorController.h"
#include "Core/GameCore/FirstPersonCamera.h"
#include "Core/GameCore/BoundingBox.h"
#include "Core/GameCore/Components/PrimitiveComponents/PrimitiveComponent.h"

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

      BoundingBox mLevelBounds;

      std::shared_ptr<PrimitiveComponent> mSpaceShipPrimitiveComponent;

   public:

      SpaceShipPlayerController(const std::shared_ptr<ACamera>& playerCamera, const std::shared_ptr<Actor>& actor);

      virtual ~SpaceShipPlayerController();

      virtual void Tick(float deltaTime) override;
      
      virtual void InitActorController() override;

      void SetLevelBounds(const BoundingBox& mLevelBounds);
   };

}

