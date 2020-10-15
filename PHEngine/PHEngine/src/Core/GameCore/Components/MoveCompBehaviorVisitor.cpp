#include "MoveCompBehaviorVisitor.h"
#include "Core/GameCore/Components/SceneComponent.h"
#include "Core/GameCore/Components/PhysicsComponents/PhysicsComponent.h"
#include "Core/UtilityCore/EngineMath.h"

using namespace EnginePhysics;

namespace Game
{

   MoveCompBehaviorVisitorBase::MoveCompBehaviorVisitorBase(std::weak_ptr<SceneComponent> ownerRootComponent)
      : mOwnerRootComp(ownerRootComponent)
   {
   }

   void MoveCompBehaviorVisitorBase::Init()
   {
      if (auto rootCompSP = mOwnerRootComp.lock())
      {
         mWorldTranslation = rootCompSP->GetTranslation();
         mWorldRotator = rootCompSP->GetRotator();
         mStartTranslation = mWorldTranslation;
         mWorldTranslationDelta = glm::vec3(0);
      }
   }

   void MoveCompBehaviorVisitorBase::LerpTranslation(const float time, const float transitionTime, const glm::vec3& finalTargetVector)
   {
      const auto prevPosition = mWorldTranslation;
      mWorldTranslation = EngineMath::LerpVec3(time, 0.0f, transitionTime, mStartTranslation, finalTargetVector);
      mWorldTranslationDelta = mWorldTranslation - prevPosition;
   }

   glm::vec3 MoveCompBehaviorVisitorBase::GetWorldTranslation() const {
      return mWorldTranslation;
   }

   glm::vec3 MoveCompBehaviorVisitorBase::GetWorldTranslationDelta() const {
      return mWorldTranslationDelta;
   }

   glm::vec3 MoveCompBehaviorVisitorBase::GetStartWorldTranslation() const {
      return mStartTranslation;
   }

   glm::quat MoveCompBehaviorVisitorBase::GetWorldRotator() const {
      return mWorldRotator;
   }

   glm::quat MoveCompBehaviorVisitorBase::GetWorldRotatorDelta() const {
      return glm::quat();
   }

   glm::quat MoveCompBehaviorVisitorBase::GetStartWorldRotator() const
   {
      return glm::quat();
   }

   MoveCompBehaviorVisitorNoPhys::MoveCompBehaviorVisitorNoPhys(std::weak_ptr<SceneComponent> ownerRootComponent)
      : MoveCompBehaviorVisitorBase(ownerRootComponent)
   {
   }

   void MoveCompBehaviorVisitorBase::CommitDestinationPointReached()
   {
      mStartTranslation = mWorldTranslation;
   }

   void MoveCompBehaviorVisitorNoPhys::CommitMove()
   {
      if (auto rootCompSP = mOwnerRootComp.lock())
      {
         rootCompSP->SetTranslation(mWorldTranslation);
      }
   }
   
   MoveCompBehaviorVisitorWithPhys::MoveCompBehaviorVisitorWithPhys(std::weak_ptr<SceneComponent> ownerRootComponent, std::weak_ptr<PhysicsComponent> ownerPhysComponent)
      : MoveCompBehaviorVisitorBase(ownerRootComponent)
      , mOwnerPhysComp(ownerPhysComponent)
   {
   }

   void MoveCompBehaviorVisitorWithPhys::CommitMove()
   {
      if (auto rootCompSP = mOwnerRootComp.lock())
      {
         rootCompSP->SetTranslation(mWorldTranslation);

         if (auto physCompSP = mOwnerPhysComp.lock())
         {
            physCompSP->SetWorldTranslation(mWorldTranslation);
         }
      }
   }
}