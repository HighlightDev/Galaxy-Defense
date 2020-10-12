#include "MovementComponent.h"

#include "Core/UtilityCore/EngineMath.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Event/KinematicBodyMovedEvent.h"
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
         mWorldTranslationDelta = glm::vec3(0);
      }

      mScriptExecutor.RegisterCallbacks();
      mScriptExecutor.RunScript();
   }

   uint64_t MovementComponent::GetComponentType() const
   {
      return COMPONENT;
   }   

   const std::unordered_map<std::string, std::tuple<Transform, float>>&  MovementComponent::GetMovementPoints() const
   {
      return mMovementPoints;
   }

   void MovementComponent::AddMovementPoint(const std::string& pointName, const Transform& t, const float transitionTime)
   {
      assert(!mMovementPoints.count(pointName));

      mMovementPoints.emplace(pointName, std::make_tuple(std::move(t), transitionTime));
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

      const glm::vec3& finalTargetVector = std::get<0>(mMovementPoints[mDestinationPoint]).Translation;
      const float transitionTime = std::get<1>(mMovementPoints[mDestinationPoint]);

      const auto prevPosition = mWorldPosition;
      mWorldPosition = EngineMath::LerpVec3(mTime, 0.0f, transitionTime, mStartPosition, finalTargetVector);
      mWorldTranslationDelta = mWorldPosition - prevPosition;

      // If camera is at final position  
      if (EngineMath::CompareFloats(mTime, transitionTime))
      {
         mTime = 0.0f;
         mDestinationPoint = "NO";
      }
      mTime = fmod(mTime, transitionTime);
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

            Transform t;
            t.Translation = mWorldTranslationDelta;
            KinematicBodyMovedEvent::GetInstance()->SendEvent(Event::ExecutionOrder::POST_EXECUTION, physCompSP->GetDescriptor(), std::move(t));
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
