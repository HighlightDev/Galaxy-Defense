#include "MovementComponent.h"

#include "Core/UtilityCore/EngineMath.h"
#include "Core/GameCore/Actor.h"
#include <iostream>

namespace Game
{

   MovementComponent::MovementComponent(const std::string& gameObjectName, const std::string& relPathToScript)
      : Component(gameObjectName)
      , mScriptExecutor(this, relPathToScript)
      , mDestinationPoint("NO")
      , mTime(0.0f)
   {
   }

   MovementComponent::~MovementComponent()
   {
   }

   void MovementComponent::PostLevelInit()
   {
      if (auto rootCompSP = GetOwner()->GetRootComponent())
      {
         mWorldPosition = rootCompSP->GetTranslation();
         mWorldRotation = rootCompSP->GetRotator();
         mStartPosition = mWorldPosition;
      }

      mScriptExecutor.RegisterCallbacks();
      mScriptExecutor.RunScript();
   }

   uint64_t MovementComponent::GetComponentType() const
   {
      return COMPONENT;
   }   

   const std::unordered_map<std::string, Transform>&  MovementComponent::GetMovementPoints() const
   {
      return mMovementPoints;
   }

   void MovementComponent::AddMovementPoint(const std::string& pointName, const Transform& t)
   {
      assert(!mMovementPoints.count(pointName));

      mMovementPoints.emplace(pointName, std::move(t));
   }

   void MovementComponent::SetDestinationPoint(const std::string& pointName)
   {
      mDestinationPoint = pointName;
      mLastDestinationPoint = pointName;
   }

   std::string MovementComponent::GetDestinationPoint() const
   {
      return mDestinationPoint;
   }

   void MovementComponent::Move(const float deltaTime)
   {
      mTime += deltaTime;

      glm::vec3 finalTargetVector = mMovementPoints[mDestinationPoint].Translation;

      mWorldPosition = EngineMath::LerpVec3(mTime, 0.0f, 2.0f, mStartPosition, finalTargetVector);

      // If camera is at final position  
      if (EngineMath::CompareFloats(mTime, 2.0f))
      {
         mTime = 0.0f;
         mDestinationPoint = "NO";
      }
      mTime = fmod(mTime, 2.0f);
   }

   void MovementComponent::Tick(const float deltaTime)
   {
      if (auto physCompSP = GetOwner()->GetPhysicsComponent())
      {
         if (mDestinationPoint != "NO")
         {
            Move(deltaTime);
            physCompSP->SetWorldTranslation(mWorldPosition);
            auto rootComp = GetOwner()->GetRootComponent();
            rootComp->SetTranslation(mWorldPosition);
         }
         else
         {
            auto itNext = (++(mMovementPoints.find(mLastDestinationPoint)));
            if (itNext == mMovementPoints.end())
               itNext = mMovementPoints.begin();

            SetDestinationPoint(itNext->first);
            mStartPosition = mWorldPosition;
         }
      }
   }
}
