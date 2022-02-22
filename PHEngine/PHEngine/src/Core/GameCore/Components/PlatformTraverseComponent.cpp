#include "PlatformTraverseComponent.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Event/KinematicBodyMovedEvent.h"
#include "Core/GameCore/Components/ComponentData/PlanarReflectionComponentData.h"

#include <iostream>

namespace EngineCore
{

   PlatformTraverseComponent::PlatformTraverseComponent(const PlatformTraverseComponentData& data)
       : Component(data.GameObjectName), mScriptExecutor(this, data.mScriptName), mDestinationPoint("NO"), mTime(0.0f)
   {
   }

   PlatformTraverseComponent::~PlatformTraverseComponent()
   {
   }

   void PlatformTraverseComponent::PostLevelInit()
   {
      if (const auto &spOwner = GetOwner().lock())
      {
         const auto &rootComponent = spOwner->GetRootComponent();
         assert(rootComponent);

         mScriptExecutor.PostInit(spOwner->GetSceneOwner());

         const auto &physComponent = spOwner->GetPhysicsComponent();

         if (physComponent)
         {
            mBehaviorVisitor = std::make_unique<PlatformTraverseComponentVisitorWithPhys>(rootComponent, physComponent);
         }
         else
         {
            mBehaviorVisitor = std::make_unique<PlatformTraverseComponentVisitorNoPhys>(rootComponent);
         }

         mBehaviorVisitor->Init();

         mScriptExecutor.RegisterCallbacks();
         mScriptExecutor.RunScript();
      }
   }

   ComponentType PlatformTraverseComponent::GetComponentType() const
   {
      return PLATFORM_MOVEMENT_COMPONENT;
   }

   const std::unordered_map<std::string, std::tuple<EulerAnglesTransform, float>> &PlatformTraverseComponent::GetMovementPoints() const
   {
      return mMovementPoints;
   }

   void PlatformTraverseComponent::AddMovementPoint(const std::string &pointName, const EulerAnglesTransform &t, const float transitionTime)
   {
      assert(!mMovementPoints.count(pointName));

      mMovementPoints.emplace(pointName, std::make_tuple(t, transitionTime));
   }

   void PlatformTraverseComponent::SetDestinationPoint(const std::string &pointName)
   {
      mDestinationPoint = pointName;
      mLastDestinationPoint = pointName;
      const EulerAnglesTransform &transform = std::get<0>(mMovementPoints[mDestinationPoint]);
      mBehaviorVisitor->CommitMovementStarted(transform);
   }

   std::string PlatformTraverseComponent::GetDestinationPoint() const
   {
      return mDestinationPoint;
   }

   void PlatformTraverseComponent::Move(const float deltaTime)
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

   void PlatformTraverseComponent::Tick(const float deltaTime)
   {
      mScriptExecutor.OnUpdate(deltaTime);

      if (mDestinationPoint != "NO")
      {
         Move(deltaTime);

         mBehaviorVisitor->CommitMove();

         EulerAnglesTransform transform;
         transform.Translation = mBehaviorVisitor->GetWorldTranslationDelta();

         if (const auto &spOwner = GetOwner().lock())
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

   void PlatformTraverseComponent::CollectDataForSerialization(SerializeDataContainer &dataContainer)
   {
      auto &actorData = GetSerializeDataActor(dataContainer);

      std::shared_ptr<SerializeDataPlatformTraverseComponent> data = std::make_shared<SerializeDataPlatformTraverseComponent>();
      data->ComponentName = GameObjectName;
      data->ScriptName = mScriptExecutor.GetScriptRelPath();

      actorData.ComponentsData.emplace_back(data);
   }
}
