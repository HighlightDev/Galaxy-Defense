#pragma once

#include <memory>
#include <glm/vec3.hpp>
#include <glm/ext/quaternion_float.hpp>
#include "Core/GameCore/Components/Transform.h"

namespace EnginePhysics
{
   class PhysicsComponent;
}

namespace Game
{
   class SceneComponent;
   using EnginePhysics::PhysicsComponent;

   class PlatformMovementComponentVisitorBase
   {
   protected:

      glm::vec3 mWorldTranslation;
      glm::vec3 mWorldTranslationDelta;

      glm::quat mWorldRotator;

      glm::vec3 mStartTranslation;
      glm::quat mStartRotator;

      glm::vec3 mEndTranslation;
      glm::quat mEndRotator;

      std::weak_ptr<SceneComponent> mOwnerRootComp;

   public:

      PlatformMovementComponentVisitorBase(std::weak_ptr<SceneComponent> ownerRootComponent);

      void Init();

      void LerpTransformation(const float time, const float transitionTime);

      glm::vec3 GetWorldTranslation() const;
      glm::vec3 GetWorldTranslationDelta() const;
      glm::vec3 GetStartWorldTranslation() const;

      glm::quat GetWorldRotator() const;
      glm::quat GetWorldRotatorDelta() const;
      glm::quat GetStartWorldRotator() const;

      void CommitMovementStarted(const EulerAnglesTransform& targetTransform);
      void CommitMovementFinished();

      virtual void CommitMove() = 0;
   };

   class PlatformMovementComponentVisitorNoPhys 
      : public PlatformMovementComponentVisitorBase
   {

   public:

      PlatformMovementComponentVisitorNoPhys(std::weak_ptr<SceneComponent> ownerRootComponent);

      virtual void CommitMove() override;
   };

   class PlatformMovementComponentVisitorWithPhys
      : public PlatformMovementComponentVisitorBase
   {

      std::weak_ptr<PhysicsComponent> mOwnerPhysComp;

   public:

      PlatformMovementComponentVisitorWithPhys(std::weak_ptr<SceneComponent> ownerRootComponent, std::weak_ptr<PhysicsComponent> ownerPhysComponent);

      virtual void CommitMove() override;
   };
}