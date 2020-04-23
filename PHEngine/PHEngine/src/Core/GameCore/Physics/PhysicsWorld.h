#pragma once

#include "Core/GameCore/ITickable.h"
#include "Core/GameCore/Event/PhysicsDescriptorRemovedEvent.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/PhysicsDescriptor.h"
#include "Core/GameCore/Physics/DebugRender/BulletDebugRenderer.h"

namespace Game
{
   class PhysicsWorld 
      : public ITickable
      , public Event::PhysicsDescriptorRemovedEvent
   {
      btBroadphaseInterface* mBroadphase;

      btDefaultCollisionConfiguration*        mCollisionConfiguration;

      btCollisionDispatcher*                  mDispatcher;

      btSequentialImpulseConstraintSolver*    mSolver;

   public:
      btDiscreteDynamicsWorld*                mWorld;
   private:
      std::vector<PhysicsDescriptor*> mPhysicsDescriptors;

   public:

      BulletDebugRenderer* DebugRenderer;

      PhysicsWorld();

      ~PhysicsWorld();

      void Tick(const float deltaTime);

      void InitPhysicsWorld();

      void AddPhysDescriptor(PhysicsDescriptor* inDescriptor);

      void RemovePhysDescriptorFromSimulation(PhysicsDescriptor* descriptor);

      void JoinPhysDescriptorsForSimulation();

   protected:

      virtual void ProcessEvent(const Event::PhysicsDescriptorRemovedEvent::EventData_t& data) override;

   };
}