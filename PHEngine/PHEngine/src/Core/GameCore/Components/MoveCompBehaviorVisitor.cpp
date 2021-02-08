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
         mStartTranslation = mWorldTranslation;
         mWorldTranslationDelta = glm::vec3(0);

         mWorldRotator = rootCompSP->GetRotator();
         mStartRotator = mWorldRotator;
      }
   }

   void MoveCompBehaviorVisitorBase::LerpTransformation(const float time, const float transitionTime)
   {
      const float currentTransitionTime = glm::clamp(time, 0.0f, transitionTime);

      const auto prevPosition = mWorldTranslation;
      mWorldTranslation = EngineMath::LerpVec3(currentTransitionTime, 0.0f, transitionTime, mStartTranslation, mEndTranslation);
      mWorldTranslationDelta = mWorldTranslation - prevPosition;

      mWorldRotator = EngineMath::SLerpQuat(currentTransitionTime / transitionTime, mStartRotator, mEndRotator);
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

   void MoveCompBehaviorVisitorBase::CommitMovementStarted(const EulerAnglesTransform& targetTransform)
   {
      mEndTranslation = mStartTranslation + targetTransform.Translation;
      mEndRotator = mStartRotator * EngineMath::EulerAnglesToQuat(targetTransform.RotationEulerAngles);
   }

   void MoveCompBehaviorVisitorBase::CommitMovementFinished()
   {
      mStartTranslation = mWorldTranslation;
      mStartRotator = mWorldRotator;
   }

   void MoveCompBehaviorVisitorNoPhys::CommitMove()
   {
      if (auto rootCompSP = mOwnerRootComp.lock())
      {
         rootCompSP->SetTranslation(mWorldTranslation);
         rootCompSP->SetRotator(mWorldRotator);
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
         rootCompSP->SetRotator(mWorldRotator);

         if (auto physCompSP = mOwnerPhysComp.lock())
         {
            physCompSP->SetWorldTranslation(mWorldTranslation);
            physCompSP->SetWorldRotator(mWorldRotator);
         }
      }
   }
}