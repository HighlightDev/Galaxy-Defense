#pragma once
#include "SceneComponent.h"
#include "Core/GameCore/Components/PhysicsComponents/PhysicsComponent.h"

using namespace EnginePhysics;

namespace Game
{

   class MovementComponent
      : public Component
   {

      std::weak_ptr<SceneComponent> mOwnerRootComponent;
      std::weak_ptr<PhysicsComponent> mOwnerPhysicsComponent;

      std::unordered_map<std::string, Transform> mMovementPoints;

      std::string mCurrentPoint;
      std::string mDestinationPoint;

      glm::vec3 mWorldPosition;
      glm::quat mWorldRotation;

      glm::vec3 mStartPosition;
      glm::quat mStartRotation;

      float mTime;

   public:

      MovementComponent(const std::string& gameObjectName);

      virtual ~MovementComponent();

      virtual uint64_t GetComponentType() const override;

      virtual void Tick(const float deltaTime) override;

      virtual void PostLevelInit() override;

   private:

      void Move(const float deltaTime);
   };

}
