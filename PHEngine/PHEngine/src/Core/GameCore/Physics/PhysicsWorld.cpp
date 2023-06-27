#include "PhysicsWorld.h"
#include "Core/ResourceManagerCore/Pool/MeshPool.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Event/PhysicsCollisionEvent.h"

using namespace Event;

namespace EnginePhysics
{

   PhysicsWorld::PhysicsWorld()
       : Event::PhysicsDescriptorRemovedEvent(),
         mBroadphase(nullptr),
         mCollisionConfiguration(nullptr),
         mDispatcher(nullptr),
         mSolver(nullptr),
         mWorld(nullptr),
#if DEBUG
         mDebugRenderer(new BulletDebugRenderer())
#endif
   {
      LogInfo("PhysicsWorld::ctor");

      Event::PhysicsDescriptorRemovedEvent::GetInstance()->AddListener(this);
   }

   PhysicsWorld::~PhysicsWorld()
   {
      LogInfo("PhysicsWorld::dctor");

      Event::PhysicsDescriptorRemovedEvent::GetInstance()->RemoveListener(this);

      for (size_t i = 0; i < mPhysicsDescriptors.size(); ++i)
      {
         RemovePhysDescriptorFromSimulation(mPhysicsDescriptors[i]);
      }
      mPhysicsDescriptors.clear();

      delete mBroadphase;
      delete mCollisionConfiguration;
      delete mDispatcher;
      delete mSolver;
#if DEBUG
      delete mDebugRenderer;
#endif
      delete mWorld;
   }

   void PhysicsWorld::InitPhysicsWorld()
   {
      mBroadphase = new btDbvtBroadphase();

      mCollisionConfiguration = new btDefaultCollisionConfiguration();
      mDispatcher = new btCollisionDispatcher(mCollisionConfiguration);

      mSolver = new btSequentialImpulseConstraintSolver();

      mWorld = new btDiscreteDynamicsWorld(mDispatcher, mBroadphase, mSolver, mCollisionConfiguration);

      mWorld->setGravity(btVector3(btScalar(0.0f), btScalar(-9.8f), btScalar(0.0f)));

#if DEBUG
      mWorld->setDebugDrawer(mDebugRenderer);
#endif
   }

   btDiscreteDynamicsWorld *PhysicsWorld::GetWorld() const
   {
      return mWorld;
   }

   void PhysicsWorld::AddPhysDescriptor(PhysicsDescriptor *inDescriptor)
   {
      LogInfo("PhysicsWorld::AddPhysDescriptor => descriptor id = ", inDescriptor->GetId());

      mPhysicsDescriptors.push_back(inDescriptor);
   }

   void PhysicsWorld::RemovePhysDescriptorFromSimulation(PhysicsDescriptor *descriptor)
   {
      LogInfo("PhysicsWorld::RemovePhysDescriptorFromSimulation => descriptor id = ", descriptor->GetId());
      // !!! ATTENTION !!!
      // this is the only place,
      // where descriptor could be deleted.
      delete descriptor;
   }

   void PhysicsWorld::Tick(const float deltaTime)
   {
      if (mPhysicsDescriptors.size())
      {
         mWorld->stepSimulation(deltaTime * 10.0f); // todo: something bad....

         PostPhysicsSimulationUpdate(deltaTime);

#if DEBUG
         mDebugRenderer->ClearLinesBuffer();
         mWorld->debugDrawWorld();
#endif
      }
   }

   void PhysicsWorld::PostPhysicsSimulationUpdate(const float deltaTime)
   {
      for (const auto &physicsDescriptor : mPhysicsDescriptors)
      {
         if (physicsDescriptor->GetIsCollisionEnabled())
         {
            physicsDescriptor->PostPhysicsSimulationUpdate(deltaTime);
         }
      }

      for (auto &activeCollision : mActiveCollisions)
      {
         activeCollision.Tick(deltaTime);

         if (activeCollision.IsCollisionExpired())
         {
            PhysicsCollisionEvent::GetInstance()->SendEvent(eExecutionOrder::POST_EXECUTION,
                                                            ePhysicsCollisionStateType::COLLISION_UNREGISTER,
                                                            activeCollision.GetFirstCollisionBody()->GetPhysicsBodyType(),
                                                            activeCollision.GetFirstCollisionBody()->GetId(),
                                                            activeCollision.GetFirstCollisionBody()->GetOwnerActorEngineObjectId(),
                                                            activeCollision.GetSecondCollisionBody()->GetId(),
                                                            activeCollision.GetSecondCollisionBody()->GetOwnerActorEngineObjectId());
         }
      }

      const auto removeIt = std::remove_if(mActiveCollisions.begin(), mActiveCollisions.end(), [](const auto &activeCollisionPair)
                                           { return activeCollisionPair.IsCollisionExpired(); });
      if (removeIt != mActiveCollisions.end())
      {
         mActiveCollisions.erase(removeIt, mActiveCollisions.end());
      }
   }

   PhysicsDescriptor *PhysicsWorld::GetPhysicsDescriptorById(const size_t descriptorId) const
   {
      PhysicsDescriptor *result = nullptr;
      auto foundDescriptorIt = std::find_if(mPhysicsDescriptors.begin(), mPhysicsDescriptors.end(), [=](const auto &physDescriptor)
                                            { return physDescriptor->GetId() == descriptorId; });

      if (foundDescriptorIt != mPhysicsDescriptors.end())
      {
         result = *foundDescriptorIt;
      }

      return result;
   }

   void PhysicsWorld::RegisterActiveCollision(const PhysicsDescriptor *collisionBody1, const PhysicsDescriptor *collisionBody2)
   {
      assert(collisionBody1->GetPhysicsBodyType() == collisionBody2->GetPhysicsBodyType());
      auto activeCollisionIt = std::find_if(mActiveCollisions.begin(),
                                            mActiveCollisions.end(),
                                            [&](const auto &collisionPair)
                                            { return ((collisionBody1->GetId() == collisionPair.GetFirstCollisionBodyId()) && (collisionBody2->GetId() == collisionPair.GetSecondCollisionBodyId())) ||
                                                     ((collisionBody1->GetId() == collisionPair.GetSecondCollisionBodyId()) && (collisionBody2->GetId() == collisionPair.GetFirstCollisionBodyId())); });

      if (activeCollisionIt == mActiveCollisions.end())
      {
         mActiveCollisions.emplace_back(collisionBody1, collisionBody2);
         PhysicsCollisionEvent::GetInstance()->SendEvent(eExecutionOrder::POST_EXECUTION,
                                                         ePhysicsCollisionStateType::COLLISION_REGISTERED,
                                                         collisionBody1->GetPhysicsBodyType(),
                                                         collisionBody1->GetId(),
                                                         collisionBody1->GetOwnerActorEngineObjectId(),
                                                         collisionBody2->GetId(),
                                                         collisionBody2->GetOwnerActorEngineObjectId());
      }
      else
      {
         activeCollisionIt->ReloadActiveCollisionLifetime();
      }
   }

#if DEBUG
   const DebugPhysicsRenderData &PhysicsWorld::GetDebugPhysicsRenderData() const
   {
      return mDebugRenderer->GetRenderData();
   }
#endif

   void PhysicsWorld::ProcessEvent(const Event::PhysicsDescriptorRemovedEvent::EventData_t &data)
   {
      auto removedDescriptorIt = std::find_if(mPhysicsDescriptors.begin(), mPhysicsDescriptors.end(), [&](const PhysicsDescriptor *physDesc)
                                              { return physDesc->GetId() == std::get<0>(data); });

      mPhysicsDescriptors.erase(removedDescriptorIt);
      RemovePhysDescriptorFromSimulation(*removedDescriptorIt);
   }
}
