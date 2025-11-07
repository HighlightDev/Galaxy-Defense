#include "GhostController.h"

#include "Core/GameCore/Components/Transform.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Physics/ActiveCollisionPair.h"
#include "Core/GameCore/Physics/PhysicsWorld.h"
#include "Core/UtilityCore/GlmToBulletConverter.h"

#include <glm/gtx/projection.hpp>

using namespace EngineCore;

namespace EnginePhysics {
GhostController::GhostController(
    const std::shared_ptr<PhysicsWorld>& pPhysicsWorld,
    const std::shared_ptr<CollisionShapeBase>& shape,
    const float mass,
    const int32_t collisionFilterGroup,
    const int32_t collisionFilterMask)
    : PhysicsDescriptor(pPhysicsWorld, shape, ePhysicsBodyType::GHOST, mass)
    , btCollisionWorld::ContactResultCallback()
    , mGhostObject(nullptr)
    , mMotionTransform()
    , mCollisionCooldown(0.0f)
{
    m_collisionFilterGroup = collisionFilterGroup;
    m_collisionFilterMask = collisionFilterMask;
    mSavedCollisionFilterGroup = collisionFilterGroup;
    mSavedCollisionFilterMask = collisionFilterMask;
}

GhostController::~GhostController()
{
    CleanUp();
}

void GhostController::CleanUp()
{
    PhysicsDescriptor::CleanUp();

    if (mGhostObject) {
        mPhysicsWorld->GetWorld()->removeCollisionObject(mGhostObject);
        delete mGhostObject;
        mGhostObject = nullptr;
    }
}

ePhysicsDescriptorType GhostController::GetPhysicsDescriptorType() const
{
    return ePhysicsDescriptorType::GHOST_CONTROLLER;
}

void GhostController::SetMotionStateWorldTransform(const btQuaternion& quat, const btVector3& translation)
{
    mMotionTransform = btTransform(btQuaternion(1.0f, 0.0f, 0.0f, 0.0f), translation);
}

void GhostController::CompletePhysicsDescriptorConstruction()
{
    mGhostObject = new btPairCachingGhostObject();

    mGhostObject->setCollisionShape(mShape->GetCollisionShape());
    mGhostObject->setUserPointer(static_cast<PhysicsDescriptor*>(this));
    mGhostObject->setCollisionFlags(btCollisionObject::CF_DYNAMIC_OBJECT);
    mPhysicsWorld->GetWorld()->addCollisionObject(mGhostObject, m_collisionFilterGroup, m_collisionFilterMask);
}

void GhostController::SetIsCollisionEnabled(const bool isCollisionEnabled)
{
    assert(mGhostObject);

    if (isCollisionEnabled != mIsCollisionEnabled) {
        if (isCollisionEnabled) {
            m_collisionFilterGroup = mSavedCollisionFilterGroup;
            m_collisionFilterMask = mSavedCollisionFilterMask;
            mGhostObject->setCollisionFlags(btCollisionObject::CF_DYNAMIC_OBJECT);
            mPhysicsWorld->GetWorld()->addCollisionObject(mGhostObject);
        } else {
            m_collisionFilterGroup = btBroadphaseProxy::DefaultFilter;
            m_collisionFilterMask = btBroadphaseProxy::SensorTrigger;
            mGhostObject->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
            mPhysicsWorld->GetWorld()->removeCollisionObject(mGhostObject);
        }
        mIsCollisionEnabled = isCollisionEnabled;
    }
}

void GhostController::UpdateMotionWorldTransformLocalState(bool& bIsWorldTransformDiry, const float deltaTimeSec)
{
    // Sync ghost with actually object
    mGhostObject->setWorldTransform(mMotionTransform);
    bIsWorldTransformDiry = false;
}

void GhostController::PostPhysicsSimulationUpdate(const float deltaTimeSec)
{
    mCollisionCooldown += deltaTimeSec;
    ParseGhostContacts();
}

std::vector<btCollisionObject*> GhostController::GetCollisionObjects() const
{
    return {mGhostObject};
}

btScalar GhostController::addSingleResult(
    btManifoldPoint& cp,
    const btCollisionObjectWrapper* colObj0,
    int partId0,
    int index0,
    const btCollisionObjectWrapper* colObj1,
    int partId1,
    int index1)
{
    const auto& collidedObject = colObj1->getCollisionObject();
    auto collidedObjDescriptor = reinterpret_cast<PhysicsDescriptor*>(collidedObject->getUserPointer());
    if (mGhostObject == collidedObject || !collidedObjDescriptor->GetIsCollisionEnabled())
        return 1.0f;

    mPhysicsWorld->RegisterActiveCollision(shared_from_this(), collidedObjDescriptor->shared_from_this());

    return 0.0f;
}

void GhostController::ParseGhostContacts()
{
    if (mCollisionCooldown > sCollisionCooldownTimeout) {
        mPhysicsWorld->GetWorld()->contactTest(mGhostObject, *this);
        mCollisionCooldown = fmod(mCollisionCooldown, sCollisionCooldownTimeout);
    }
}
} // namespace EnginePhysics