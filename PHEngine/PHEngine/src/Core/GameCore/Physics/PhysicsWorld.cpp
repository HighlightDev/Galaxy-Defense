#include "PhysicsWorld.h"

#include "Core/GameCore/Event/PhysicsCollisionEvent.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/ResourceManagerCore/Pool/MeshPool.h"

using namespace Event;

namespace EnginePhysics {

PhysicsWorld::PhysicsWorld()
    : Event::PhysicsDescriptorRemovedGameThreadEvent()
    , mBroadphase(nullptr)
    , mCollisionConfiguration(nullptr)
    , mDispatcher(nullptr)
    , mSolver(nullptr)
    , mWorld(nullptr)
    ,
#if DEBUG
    mDebugRenderer(new BulletDebugRenderer())
#endif
{
    LogInfo("PhysicsWorld::ctor");
}

PhysicsWorld::~PhysicsWorld()
{
    LogInfo("PhysicsWorld::dctor");

    Event::PhysicsDescriptorRemovedGameThreadEvent::GetInstance()->RemoveListener(
        Event::PhysicsDescriptorRemovedGameThreadEvent::GetInstanceId());
    for (const auto physDescriptor : mPhysicsDescriptors) {
        physDescriptor->CleanUp();
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

void PhysicsWorld::Initialize()
{
    Event::PhysicsDescriptorRemovedGameThreadEvent::GetInstance()->AddListener(shared_from_this());

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

void PhysicsWorld::UnloadExistingPhysicsSimulation()
{
    LogInfo("PhysicsWorld::UnloadExistingPhysicsSimulation: descriptors count: ", mPhysicsDescriptors.size());
    mActiveCollisions.clear();
    mPhysicsDescriptors.clear();
}

btDiscreteDynamicsWorld* PhysicsWorld::GetWorld() const
{
    return mWorld;
}

void PhysicsWorld::AddPhysDescriptor(const std::shared_ptr<PhysicsDescriptor>& inDescriptor)
{
    LogInfo("PhysicsWorld::AddPhysDescriptor: descriptor id = ", inDescriptor->GetId());

    mPhysicsDescriptors.emplace_back(inDescriptor);
}

void PhysicsWorld::RemovePhysDescriptorFromSimulation(const std::shared_ptr<PhysicsDescriptor>& descriptor)
{
    LogInfo("PhysicsWorld::RemovePhysDescriptorFromSimulation: descriptor id = ", descriptor->GetId());
    auto removedDescriptorIt = std::remove_if(
        mPhysicsDescriptors.begin(), mPhysicsDescriptors.end(), [id = descriptor->GetId()](const auto& physDesc) {
            return physDesc->GetId() == id;
        });

    mPhysicsDescriptors.erase(removedDescriptorIt);
}

void PhysicsWorld::Tick(const float deltaTimeSec)
{
    if (mPhysicsDescriptors.size()) {
        mWorld->stepSimulation(deltaTimeSec * 10.0f);

        PostPhysicsSimulationUpdate(deltaTimeSec);

#if DEBUG
        mDebugRenderer->ClearLinesBuffer();
        mWorld->debugDrawWorld();
#endif
    }
}

void PhysicsWorld::PostPhysicsSimulationUpdate(const float deltaTimeSec)
{
    for (const auto& physicsDescriptor : mPhysicsDescriptors) {
        if (physicsDescriptor->GetIsCollisionEnabled()) {
            physicsDescriptor->PostPhysicsSimulationUpdate(deltaTimeSec);
        }
    }

    for (auto& activeCollision : mActiveCollisions) {
        activeCollision.Tick(deltaTimeSec);

        if (activeCollision.IsCollisionExpired()) {
            PhysicsCollisionGameThreadEvent::GetInstance()->SendEvent(
                eExecutionOrder::POST_EXECUTION,
                ePhysicsCollisionStateType::COLLISION_UNREGISTER,
                activeCollision.GetFirstCollisionBodyType(),
                activeCollision.GetFirstCollisionBodyId(),
                activeCollision.GetFirstCollisionBodyOwnerActorObjectId(),
                activeCollision.GetSecondCollisionBodyId(),
                activeCollision.GetSecondCollisionBodyOwnerActorObjectId());
        }
    }

    const auto removeIt = std::remove_if(mActiveCollisions.begin(), mActiveCollisions.end(), [](const auto& activeCollisionPair) {
        return activeCollisionPair.IsCollisionExpired();
    });
    mActiveCollisions.erase(removeIt, mActiveCollisions.end());
}

std::shared_ptr<PhysicsDescriptor> PhysicsWorld::GetPhysicsDescriptorById(const size_t descriptorId) const
{
    std::shared_ptr<PhysicsDescriptor> result;
    auto foundDescriptorIt
        = std::find_if(mPhysicsDescriptors.begin(), mPhysicsDescriptors.end(), [descriptorId](const auto& physDescriptor) {
              return physDescriptor->GetId() == descriptorId;
          });

    if (foundDescriptorIt != mPhysicsDescriptors.end()) {
        result = *foundDescriptorIt;
    }

    return result;
}

void PhysicsWorld::RegisterActiveCollision(
    const std::shared_ptr<PhysicsDescriptor>& collisionBody1Sp, const std::shared_ptr<PhysicsDescriptor>& collisionBody2Sp)
{
    if (collisionBody1Sp && collisionBody2Sp) {
        ext_assert(
            collisionBody1Sp->GetPhysicsBodyType() == collisionBody2Sp->GetPhysicsBodyType(),
            "PhysicsWorld::RegisterActiveCollision: collision bodies must be of the same type");
        auto activeCollisionIt = std::find_if(mActiveCollisions.begin(), mActiveCollisions.end(), [&](const auto& collisionPair) {
            if (collisionPair.GetFirstCollisionBody().expired() || collisionPair.GetSecondCollisionBody().expired()) {
                return false;
            }
            return ((collisionBody1Sp->GetId() == collisionPair.GetFirstCollisionBodyId())
                    && (collisionBody2Sp->GetId() == collisionPair.GetSecondCollisionBodyId()))
                || ((collisionBody1Sp->GetId() == collisionPair.GetSecondCollisionBodyId())
                    && (collisionBody2Sp->GetId() == collisionPair.GetFirstCollisionBodyId()));
        });

        if (activeCollisionIt == mActiveCollisions.end()) {
            mActiveCollisions.emplace_back(collisionBody1Sp, collisionBody2Sp);
            PhysicsCollisionGameThreadEvent::GetInstance()->SendEvent(
                eExecutionOrder::POST_EXECUTION,
                ePhysicsCollisionStateType::COLLISION_REGISTERED,
                collisionBody1Sp->GetPhysicsBodyType(),
                collisionBody1Sp->GetId(),
                collisionBody1Sp->GetOwnerActorEngineObjectId(),
                collisionBody2Sp->GetId(),
                collisionBody2Sp->GetOwnerActorEngineObjectId());
        } else {
            activeCollisionIt->ReloadActiveCollisionLifetime();
        }
    }
}

#if DEBUG
const DebugPhysicsRenderData& PhysicsWorld::GetDebugPhysicsRenderData() const
{
    return mDebugRenderer->GetRenderData();
}
#endif

void PhysicsWorld::ProcessEvent(
    const PhysicsDescriptorRemovedGameThreadEvent* sender,
    const Event::PhysicsDescriptorRemovedGameThreadEvent::EventData_t& data)
{
    if (mPhysicsDescriptors.size()) {
        const auto removedDescriptorIt
            = std::find_if(mPhysicsDescriptors.cbegin(), mPhysicsDescriptors.cend(), [&](const auto& physDesc) {
                  return physDesc->GetId() == std::get<0>(data);
              });

        if (removedDescriptorIt != mPhysicsDescriptors.cend()) {
            RemovePhysDescriptorFromSimulation(*removedDescriptorIt);
        }
    }
}
} // namespace EnginePhysics
