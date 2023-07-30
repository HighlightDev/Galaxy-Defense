#pragma once

#include "ActiveCollisionPair.h"
#include "Core/GameCore/ITickable.h"
#include "Core/GameCore/Event/PhysicsDescriptorRemovedEvent.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/PhysicsDescriptor.h"
#include "Core/GameCore/Physics/DebugRender/BulletDebugRenderer.h"

#include <vector>
#include <memory>

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

            std::vector<std::shared_ptr<PhysicsDescriptor>> mPhysicsDescriptors;

#if DEBUG
            BulletDebugRenderer *mDebugRenderer;
#endif
            std::vector<ActiveCollisionPair> mActiveCollisions;

      public:
            PhysicsWorld();

            ~PhysicsWorld();

            btDiscreteDynamicsWorld *GetWorld() const;

            void Tick(const float deltaTime);

            void UnpausableTick(const float deltaTime) override{};

            void PostPhysicsSimulationUpdate(const float deltaTime);

            void InitPhysicsWorld();

            void AddPhysDescriptor(const std::shared_ptr<PhysicsDescriptor> &inDescriptor);

            void RemovePhysDescriptorFromSimulation(const std::shared_ptr<PhysicsDescriptor> &descriptor);

            std::shared_ptr<PhysicsDescriptor> GetPhysicsDescriptorById(const size_t descriptorId) const;

            void RegisterActiveCollision(const std::shared_ptr<PhysicsDescriptor> &collisionBody1, const std::shared_ptr<PhysicsDescriptor> &collisionBody2);

#if DEBUG
            const DebugPhysicsRenderData &GetDebugPhysicsRenderData() const;
#endif

      protected:
            void ProcessEvent(const typename Event::PhysicsDescriptorRemovedEvent::EventData_t &data) override;
      };
}