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
      const auto& rootComponent = GetOwner()->GetRootComponent();
      assert(rootComponent);

      const auto& physComponent = GetOwner()->GetPhysicsComponent();

      if (physComponent)
      {
         mBehaviorVisitor = std::make_unique<MoveCompBehaviorVisitorWithPhys>(rootComponent, physComponent);
      }
      else
      {
         mBehaviorVisitor = std::make_unique<MoveCompBehaviorVisitorNoPhys>(rootComponent);
      }

      mBehaviorVisitor->Init();

      mScriptExecutor.RegisterCallbacks();
      mScriptExecutor.RunScript();
   }

   ComponentType MovementComponent::GetComponentType() const
   {
      return MOVEMENT_COMPONENT;
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

      mBehaviorVisitor->LerpTranslation(mTime, transitionTime, finalTargetVector);

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
            
            mBehaviorVisitor->CommitMove();

            Transform t;
            t.Translation = mBehaviorVisitor->GetWorldTranslationDelta();
            KinematicBodyMovedEvent::GetInstance()->SendEvent(Event::ExecutionOrder::POST_EXECUTION, physCompSP->GetDescriptor(), std::move(t));
         }
         else
         {
            if (mMovementPoints.size())
            {
               auto itNext = (++(mMovementPoints.find(mLastDestinationPoint)));
               if (itNext == mMovementPoints.end())
                  itNext = mMovementPoints.begin();

               SetDestinationPoint(itNext->first);
               mBehaviorVisitor->CommitDestinationPointReached();
            }
         }
      }
   }

   void MovementComponent::CollectDataForSerialization(SerializeDataContainer& dataContainer) 
   {

   }
}
