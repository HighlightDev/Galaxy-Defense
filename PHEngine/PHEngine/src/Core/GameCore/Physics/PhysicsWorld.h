#pragma once

#include "Core/GameCore/ITickable.h"
#include "Core/GameCore/Event/PhysicsDescriptorRemovedEvent.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/PhysicsDescriptor.h"
#include "Core/GameCore/Physics/DebugRender/BulletDebugRenderer.h"

namespace EnginePhysics
{
   class PhysicsWorld 
      : public ITickable
      , public Event::PhysicsDescriptorRemovedEvent
   {
      btBroadphaseInterface* mBroadphase;

      btDefaultCollisionConfiguration*        mCollisionConfiguration;

      btCollisionDispatcher*                  mDispatcher;

      btSequentialImpulseConstraintSolver*    mSolver;

      btDiscreteDynamicsWorld*                mWorld;

   private:
      std::vector<PhysicsDescriptor*> mPhysicsDescriptors;

#if DEBUG
      BulletDebugRenderer* mDebugRenderer;
#endif

   public:

      PhysicsWorld();

      ~PhysicsWorld();

      btDiscreteDynamicsWorld* GetWorld() const;

      void Tick(const float deltaTime);

      void PostPhysicsSimulationUpdate();

      void InitPhysicsWorld();

      void AddPhysDescriptor(PhysicsDescriptor* inDescriptor);

      void RemovePhysDescriptorFromSimulation(PhysicsDescriptor* descriptor);

#if DEBUG
      const DebugPhysicsRenderData& GetDebugPhysicsRenderData() const;
#endif

   protected:

      virtual void ProcessEvent(const typename Event::PhysicsDescriptorRemovedEvent::EventData_t& data) override;

   };
}