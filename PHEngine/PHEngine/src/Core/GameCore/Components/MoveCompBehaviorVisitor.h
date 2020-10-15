#pragma once

#include <memory>
#include <glm/vec3.hpp>
#include <glm/ext/quaternion_float.hpp>

namespace EnginePhysics
{
   class PhysicsComponent;
}

namespace Game
{
   class SceneComponent;
   using EnginePhysics::PhysicsComponent;

   class MoveCompBehaviorVisitorBase
   {
   protected:

      glm::vec3 mWorldTranslation;
      glm::vec3 mWorldTranslationDelta;

      glm::quat mWorldRotator;

      glm::vec3 mStartTranslation;
      glm::quat mStartRotator;

      std::weak_ptr<SceneComponent> mOwnerRootComp;

   public:

      MoveCompBehaviorVisitorBase(std::weak_ptr<SceneComponent> ownerRootComponent);

      void Init();

      void LerpTranslation(const float time, const float transitionTime, const glm::vec3& finalTargetVector);

      glm::vec3 GetWorldTranslation() const;
      glm::vec3 GetWorldTranslationDelta() const;
      glm::vec3 GetStartWorldTranslation() const;

      glm::quat GetWorldRotator() const;
      glm::quat GetWorldRotatorDelta() const;
      glm::quat GetStartWorldRotator() const;

      void CommitDestinationPointReached();

      virtual void CommitMove() = 0;
   };

   class MoveCompBehaviorVisitorNoPhys 
      : public MoveCompBehaviorVisitorBase
   {

   public:

      MoveCompBehaviorVisitorNoPhys(std::weak_ptr<SceneComponent> ownerRootComponent);

      virtual void CommitMove() override;
   };

   class MoveCompBehaviorVisitorWithPhys
      : public MoveCompBehaviorVisitorBase
   {

      std::weak_ptr<PhysicsComponent> mOwnerPhysComp;

   public:

      MoveCompBehaviorVisitorWithPhys(std::weak_ptr<SceneComponent> ownerRootComponent, std::weak_ptr<PhysicsComponent> ownerPhysComponent);

      virtual void CommitMove() override;
   };
}