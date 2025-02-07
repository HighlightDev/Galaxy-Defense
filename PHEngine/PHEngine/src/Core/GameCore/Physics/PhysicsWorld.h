#pragma once

#include "ActiveCollisionPair.h"
#include "Core/GameCore/Event/PhysicsDescriptorRemovedEvent.h"
#include "Core/GameCore/ITickable.h"
#include "Core/GameCore/Physics/DebugRender/BulletDebugRenderer.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/PhysicsDescriptor.h"

#include <memory>
#include <vector>

namespace EnginePhysics {
class PhysicsWorld : public ITickable,
                     public Event::PhysicsDescriptorRemovedGameThreadEvent,
                     public std::enable_shared_from_this<PhysicsWorld> {
    btBroadphaseInterface* mBroadphase;

    btDefaultCollisionConfiguration* mCollisionConfiguration;

    btCollisionDispatcher* mDispatcher;

    btSequentialImpulseConstraintSolver* mSolver;

    btDiscreteDynamicsWorld* mWorld;

    std::vector<std::shared_ptr<PhysicsDescriptor>> mPhysicsDescriptors;

#if DEBUG
    BulletDebugRenderer* mDebugRenderer;
#endif
    std::vector<ActiveCollisionPair> mActiveCollisions;

public:
    PhysicsWorld();

    ~PhysicsWorld();

    void Initialize();

    btDiscreteDynamicsWorld* GetWorld() const;

    void Tick(const float deltaTime);

    void UnpausableTick(const float deltaTime) override { };

    void PostPhysicsSimulationUpdate(const float deltaTime);

    void AddPhysDescriptor(const std::shared_ptr<PhysicsDescriptor>& inDescriptor);

    void RemovePhysDescriptorFromSimulation(const std::shared_ptr<PhysicsDescriptor>& descriptor);

    std::shared_ptr<PhysicsDescriptor> GetPhysicsDescriptorById(const size_t descriptorId) const;

    void RegisterActiveCollision(
        const std::shared_ptr<PhysicsDescriptor>& collisionBody1, const std::shared_ptr<PhysicsDescriptor>& collisionBody2);

    void UnloadExistingPhysicsSimulation();

#if DEBUG
    const DebugPhysicsRenderData& GetDebugPhysicsRenderData() const;
#endif

protected:
    void ProcessEvent(
        const PhysicsDescriptorRemovedGameThreadEvent* sender,
        const typename Event::PhysicsDescriptorRemovedGameThreadEvent::EventData_t& data) override;
};
} // namespace EnginePhysics