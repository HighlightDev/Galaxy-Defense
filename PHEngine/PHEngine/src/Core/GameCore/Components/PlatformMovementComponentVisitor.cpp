#include "PlatformMovementComponentVisitor.h"
#include "Core/GameCore/Components/SceneComponent.h"
#include "Core/GameCore/Components/PhysicsComponents/PhysicsComponent.h"
#include "Core/UtilityCore/EngineMath.h"

using namespace EnginePhysics;

namespace Game
{

   PlatformMovementComponentVisitorBase::PlatformMovementComponentVisitorBase(std::weak_ptr<SceneComponent> ownerRootComponent)
      : mOwnerRootComp(ownerRootComponent)
   {
   }

   void PlatformMovementComponentVisitorBase::Init()
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

   void PlatformMovementComponentVisitorBase::LerpTransformation(const float time, const float transitionTime)
   {
      const float currentTransitionTime = glm::clamp(time, 0.0f, transitionTime);

      const auto prevPosition = mWorldTranslation;
      mWorldTranslation = EngineMath::LerpVec3(currentTransitionTime, 0.0f, transitionTime, mStartTranslation, mEndTranslation);
      mWorldTranslationDelta = mWorldTranslation - prevPosition;

      mWorldRotator = EngineMath::SLerpQuat(currentTransitionTime / transitionTime, mStartRotator, mEndRotator);
   }

   glm::vec3 PlatformMovementComponentVisitorBase::GetWorldTranslation() const {
      return mWorldTranslation;
   }

   glm::vec3 PlatformMovementComponentVisitorBase::GetWorldTranslationDelta() const {
      return mWorldTranslationDelta;
   }

   glm::vec3 PlatformMovementComponentVisitorBase::GetStartWorldTranslation() const {
      return mStartTranslation;
   }

   glm::quat PlatformMovementComponentVisitorBase::GetWorldRotator() const {
      return mWorldRotator;
   }

   glm::quat PlatformMovementComponentVisitorBase::GetWorldRotatorDelta() const {
      return glm::quat();
   }

   glm::quat PlatformMovementComponentVisitorBase::GetStartWorldRotator() const
   {
      return glm::quat();
   }

   PlatformMovementComponentVisitorNoPhys::PlatformMovementComponentVisitorNoPhys(std::weak_ptr<SceneComponent> ownerRootComponent)
      : PlatformMovementComponentVisitorBase(ownerRootComponent)
   {
   }

   void PlatformMovementComponentVisitorBase::CommitMovementStarted(const EulerAnglesTransform& targetTransform)
   {
      mEndTranslation = mStartTranslation + targetTransform.Translation;
      mEndRotator = mStartRotator * EngineMath::EulerAnglesToQuat(targetTransform.RotationEulerAngles);
   }

   void PlatformMovementComponentVisitorBase::CommitMovementFinished()
   {
      mStartTranslation = mWorldTranslation;
      mStartRotator = mWorldRotator;
   }

   void PlatformMovementComponentVisitorNoPhys::CommitMove()
   {
      if (auto rootCompSP = mOwnerRootComp.lock())
      {
         rootCompSP->SetTranslation(mWorldTranslation);
         rootCompSP->SetRotator(mWorldRotator);
      }
   }
   
   PlatformMovementComponentVisitorWithPhys::PlatformMovementComponentVisitorWithPhys(std::weak_ptr<SceneComponent> ownerRootComponent, std::weak_ptr<PhysicsComponent> ownerPhysComponent)
      : PlatformMovementComponentVisitorBase(ownerRootComponent)
      , mOwnerPhysComp(ownerPhysComponent)
   {
   }

   void PlatformMovementComponentVisitorWithPhys::CommitMove()
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