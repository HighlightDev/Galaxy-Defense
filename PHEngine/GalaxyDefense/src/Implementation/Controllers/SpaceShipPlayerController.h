#pragma once

#include "Core/GameCore/Actor.h"
#include "Core/GameCore/ActorController.h"
#include "Core/GameCore/FirstPersonCamera.h"
#include "Core/GameCore/BoundingBox3D.h"
#include "Core/GameCore/Components/PrimitiveComponents/PrimitiveComponent.h"

using namespace Event;
using namespace EngineCore;

namespace Game
{

   class SpaceShipPlayerController
      : public ActorController
   {
      std::string mCurrentState;

      BoundingBox3D mLevelBounds;

      std::shared_ptr<PrimitiveComponent> mSpaceShipPrimitiveComponent;

   public:

      SpaceShipPlayerController(const std::shared_ptr<Actor>& actor);

      ~SpaceShipPlayerController() override;

      void Tick(const float deltaTime) override;
   
      void Initialize() override;

      void SetLevelBounds(const BoundingBox3D& mLevelBounds);
   };

}

