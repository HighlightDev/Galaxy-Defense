#pragma once

#include "ActiveCollisionPair.h"
#include "Core/GameCore/ITickable.h"
#include "Core/GameCore/Event/PhysicsDescriptorRemovedEvent.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/PhysicsDescriptor.h"
#include "Core/GameCore/Physics/DebugRender/BulletDebugRenderer.h"

#include <vector>

namespace EnginePhysics
{
   class PhysicsWorld
       : public ITickable,
         public Event::PhysicsDescriptorRemovedEvent
   {
      btBroadphaseInterface *mBroadphase;

      btDefaultCollisionConfiguration *mCollisionConfiguration;

      btCollisionDispatcher *mDispatcher;

      btSequentialImpulseConstraintSolver *mSolver;

      btDiscreteDynamicsWorld *mWorld;

      std::vector<PhysicsDescriptor *> mPhysicsDescriptors;

#if DEBUG
      BulletDebugRenderer *mDebugRenderer;
#endif
      std::vector<ActiveCollisionPair> mActiveCollisions;

   public:
      PhysicsWorld();

      ~PhysicsWorld();

      btDiscreteDynamicsWorld *GetWorld() const;

      void Tick(const float deltaTime);

      void PostPhysicsSimulationUpdate(const float deltaTime);

      void InitPhysicsWorld();

      void AddPhysDescriptor(PhysicsDescriptor *inDescriptor);

      void RemovePhysDescriptorFromSimulation(PhysicsDescriptor *descriptor);

      PhysicsDescriptor *GetPhysicsDescriptorById(const size_t descriptorId) const;

      void RegisterActiveCollision(const PhysicsDescriptor* collisionBody1, const PhysicsDescriptor* collisionBody2);

#if DEBUG
      const DebugPhysicsRenderData &GetDebugPhysicsRenderData() const;
#endif

   protected:
      virtual void ProcessEvent(const typename Event::PhysicsDescriptorRemovedEvent::EventData_t &data) override;
   };
}