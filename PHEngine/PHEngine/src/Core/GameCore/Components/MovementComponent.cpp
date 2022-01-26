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

      mScriptExecutor.PostInit(GetOwner()->GetSceneOwner());

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

   const std::unordered_map<std::string, std::tuple<EulerAnglesTransform, float>>&  MovementComponent::GetMovementPoints() const
   {
      return mMovementPoints;
   }

   void MovementComponent::AddMovementPoint(const std::string& pointName, const EulerAnglesTransform& t, const float transitionTime)
   {
      assert(!mMovementPoints.count(pointName));

      mMovementPoints.emplace(pointName, std::make_tuple(t, transitionTime));
   }

   void MovementComponent::SetDestinationPoint(const std::string& pointName)
   {
      mDestinationPoint = pointName;
      mLastDestinationPoint = pointName;
      const EulerAnglesTransform& transform = std::get<0>(mMovementPoints[mDestinationPoint]);
      mBehaviorVisitor->CommitMovementStarted(transform);
   }

   std::string MovementComponent::GetDestinationPoint() const
   {
      return mDestinationPoint;
   }

   void MovementComponent::Move(const float deltaTime)
   {
      mTime += deltaTime;

      const float transitionTime = std::get<1>(mMovementPoints[mDestinationPoint]);

      mBehaviorVisitor->LerpTransformation(mTime, transitionTime);

      // If component is at final time position  
      if (EngineMath::CompareFloats(mTime, transitionTime))
      {
         mTime = 0.0f;
         mDestinationPoint = "NO";
      }
      mTime = fmod(mTime, transitionTime);
   }

   void MovementComponent::Tick(const float deltaTime)
   {
      mScriptExecutor.OnUpdate(deltaTime);

      if (mDestinationPoint != "NO")
      {
         Move(deltaTime);

         mBehaviorVisitor->CommitMove();

         EulerAnglesTransform transform;
         transform.Translation = mBehaviorVisitor->GetWorldTranslationDelta();

         if (auto physCompSP = GetOwner()->GetPhysicsComponent())
         {
            const auto physComp = physCompSP->GetDescriptor();
            KinematicBodyMovedEvent::GetInstance()->SendEvent(Event::ExecutionOrder::POST_EXECUTION, physComp, transform);
         }
      }
      else
      {
         if (mMovementPoints.size())
         {
            auto itNext = (++(mMovementPoints.find(mLastDestinationPoint)));
            if (itNext == mMovementPoints.end())
               itNext = mMovementPoints.begin();

            mBehaviorVisitor->CommitMovementFinished();
            SetDestinationPoint(itNext->first);
         }
      }
   }

   void MovementComponent::CollectDataForSerialization(SerializeDataContainer& dataContainer) 
   {
      auto& actorData = GetSerializeDataActor(dataContainer);

      std::shared_ptr<SerializeDataMovementComponent> data = std::make_shared<SerializeDataMovementComponent>();
      data->ComponentName = GameObjectName;
      data->ScriptName= mScriptExecutor.GetScriptRelPath();

      actorData.ComponentsData.emplace_back(data);
   }
}
