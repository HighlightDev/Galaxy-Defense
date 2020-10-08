#include "MovementComponent.h"

#include "Core/UtilityCore/EngineMath.h"
#include "Core/GameCore/Actor.h"
#include <iostream>

namespace Game
{

   MovementComponent::MovementComponent(const std::string& gameObjectName)
      : Component(gameObjectName)
      , mCurrentPoint("INIT")
      , mDestinationPoint("UP1")
      , mTime(0.0f)
   {
      mMovementPoints["UP1"] = Transform(glm::vec3(0, 40, 0), glm::quat(), glm::vec3(1));
      mMovementPoints["LEFT1"] = Transform(glm::vec3(20, 40, 0), glm::quat(), glm::vec3(1));
      mMovementPoints["RIGHT1"] = Transform(glm::vec3(0, 40, 0), glm::quat(), glm::vec3(1));
      mMovementPoints["DOWN1"] = Transform(glm::vec3(0, 20, 0), glm::quat(), glm::vec3(1));
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
   }

   uint64_t MovementComponent::GetComponentType() const
   {
      return COMPONENT;
   }   
   int counter = -1;

   void MovementComponent::Move(const float deltaTime)
   {
      mTime += deltaTime;

      glm::vec3 finalTargetVector = mMovementPoints[mDestinationPoint].Translation;

      mWorldPosition = EngineMath::LerpVec3(mTime, 0.0f, 5.0f, mStartPosition, finalTargetVector);

      // If camera is at final position  
      if (EngineMath::CompareFloats(mTime, 5.0f))
      {
         mTime = 0.0f;
         mDestinationPoint = "NO";
         counter++;
      }
      mTime = fmod(mTime, 5.0f);
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
            if (counter == 0) mDestinationPoint = "LEFT1";
            else if (counter == 1) mDestinationPoint = "RIGHT1";
            else if (counter == 2) mDestinationPoint = "DOWN1";
            else if (counter == 3)
            {
               mDestinationPoint = "UP1";
               counter = -1;
            }
            mStartPosition = mWorldPosition;
         }
      }
   }
}
