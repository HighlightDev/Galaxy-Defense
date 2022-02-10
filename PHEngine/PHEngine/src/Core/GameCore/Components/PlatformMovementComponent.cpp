#include "PlatformMovementComponent.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Event/KinematicBodyMovedEvent.h"

#include "Core/CommonCore/TimeHelper.h"

#include <iostream>

namespace Game
{

   PlatformMovementComponent::PlatformMovementComponent(const std::string &gameObjectName, const std::string &relPathToScript)
       : Component(gameObjectName), mScriptExecutor(this, relPathToScript), mDestinationPoint("NO"), mTime(0.0f)
   {
   }

   PlatformMovementComponent::~PlatformMovementComponent()
   {
   }

   void PlatformMovementComponent::PostLevelInit()
   {
      if (const auto& spOwner = GetOwner().lock())
      {
         const auto &rootComponent = spOwner->GetRootComponent();
         assert(rootComponent);

         mScriptExecutor.PostInit(spOwner->GetSceneOwner());

         const auto &physComponent = spOwner->GetPhysicsComponent();

         if (physComponent)
         {
            mBehaviorVisitor = std::make_unique<PlatformMovementComponentVisitorWithPhys>(rootComponent, physComponent);
         }
         else
         {
            mBehaviorVisitor = std::make_unique<PlatformMovementComponentVisitorNoPhys>(rootComponent);
         }

         mBehaviorVisitor->Init();

         mScriptExecutor.RegisterCallbacks();
         mScriptExecutor.RunScript();
      }
   }

   ComponentType PlatformMovementComponent::GetComponentType() const
   {
      return PLATFORM_MOVEMENT_COMPONENT;
   }

   const std::unordered_map<std::string, std::tuple<EulerAnglesTransform, float>> &PlatformMovementComponent::GetMovementPoints() const
   {
      return mMovementPoints;
   }

   void PlatformMovementComponent::AddMovementPoint(const std::string &pointName, const EulerAnglesTransform &t, const float transitionTime)
   {
      assert(!mMovementPoints.count(pointName));

      mMovementPoints.emplace(pointName, std::make_tuple(t, transitionTime));
   }

   void PlatformMovementComponent::SetDestinationPoint(const std::string &pointName)
   {
      mDestinationPoint = pointName;
      mLastDestinationPoint = pointName;
      const EulerAnglesTransform &transform = std::get<0>(mMovementPoints[mDestinationPoint]);
      mBehaviorVisitor->CommitMovementStarted(transform);
   }

   std::string PlatformMovementComponent::GetDestinationPoint() const
   {
      return mDestinationPoint;
   }

   void PlatformMovementComponent::Move(const float deltaTime)
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
   #include <iostream>
   void PlatformMovementComponent::Tick(const float deltaTime)
   {
      const auto& timeBefore = EngineTime::GetCurrentTime();
      mScriptExecutor.OnUpdate(deltaTime);
      const auto& timeAfterInSec = EngineTime::GetSecondsFromDuration(EngineTime::GetPassedDuration(timeBefore));
      std::cout << "time spent on Update = " << timeAfterInSec << std::endl;

      if (mDestinationPoint != "NO")
      {
         Move(deltaTime);

         mBehaviorVisitor->CommitMove();

         EulerAnglesTransform transform;
         transform.Translation = mBehaviorVisitor->GetWorldTranslationDelta();

         if (const auto& spOwner = GetOwner().lock())
         {
            if (auto physCompSP = spOwner->GetPhysicsComponent())
            {
               const auto physDescriptor = physCompSP->GetDescriptor();
               KinematicBodyMovedEvent::GetInstance()->SendEvent(Event::ExecutionOrder::POST_EXECUTION, physDescriptor, transform);
            }
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

   void PlatformMovementComponent::CollectDataForSerialization(SerializeDataContainer &dataContainer)
   {
      auto &actorData = GetSerializeDataActor(dataContainer);

      std::shared_ptr<SerializeDataPlatformMovementComponent> data = std::make_shared<SerializeDataPlatformMovementComponent>();
      data->ComponentName = GameObjectName;
      data->ScriptName = mScriptExecutor.GetScriptRelPath();

      actorData.ComponentsData.emplace_back(data);
   }
}
