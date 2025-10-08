#include "PhysicsDescriptor.h"

#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/PhysicsBodyType.h"
#include "Core/GameCore/Physics/PhysicsWorld.h"
#include "Core/UtilityCore/EngineMath.h"

#include <limits>

using namespace EngineMath;
using namespace EngineCore;

namespace EnginePhysics {
MotionModifiers::MotionModifiers()
    : LinearFactor(btVector3(1.0f, 1.0f, 1.0f))
    , AngularFactor(btVector3(1.0f, 1.0f, 1.0f))
{
}

MotionModifiers::MotionModifiers(const btVector3& linearFactor, const btVector3& angularFactor)
    : LinearFactor(linearFactor)
    , AngularFactor(angularFactor)
{
}

int32_t PhysicsDescriptor::mTotalIds = 0;

PhysicsDescriptor::PhysicsDescriptor(
    const std::shared_ptr<PhysicsWorld>& pPhysicsWorld,
    const std::shared_ptr<CollisionShapeBase>& shape,
    const ePhysicsBodyType bodyType,
    const float mass,
    const MotionModifiers& motionModifier)
    : mBodyType(bodyType)
    , mPhysicsWorld(pPhysicsWorld)
    , mCurrentId(PhysicsDescriptor::mTotalIds++)
    , mOwnerComponentEngineObjectId(-1)
    , mOwnerActorEngineObjectId(-1)
    , mShape(shape)
    , mMotionState(new btDefaultMotionState())
    , mMass(mass)
    , mInertia(0.0f, 0.0f, 0.0f)
    , mRigidBody(nullptr)
    , mRotator()
    , mTranslation()
    , mVelocity()
    , mPrevTransform()
    , mMotionModifier(motionModifier)
    , mIsCollisionEnabled(true)
{
    if (!FloatsNearEqual(mass, 0.0f)) {
        mShape->GetCollisionShape()->calculateLocalInertia(mass, mInertia);
    }
    LogInfo(
        "PhysicsDescriptor::ctor: id: ",
        mCurrentId,
        " bodyType: ",
        ToString(mBodyType),
        " mass: ",
        mMass,
        " inertia: ",
        mInertia.x(),
        ",",
        mInertia.y(),
        ",",
        mInertia.z());
}

PhysicsDescriptor::~PhysicsDescriptor()
{
    LogInfo("PhysicsDescriptor::dtor: id: ", mCurrentId);
    CleanUp();
}

void PhysicsDescriptor::Initialize()
{
}

void PhysicsDescriptor::CleanUp()
{
    if (mRigidBody) {
        mPhysicsWorld->GetWorld()->removeCollisionObject(mRigidBody);
        delete mRigidBody;
    }

    if (mMotionState) {
        delete mMotionState;
        mMotionState = nullptr;
    }
}

void PhysicsDescriptor::PostPhysicsSimulationUpdate(const float deltaTime)
{
}

std::vector<btCollisionObject*> PhysicsDescriptor::GetCollisionObjects() const
{
    return {mRigidBody};
}

float PhysicsDescriptor::GetMass() const
{
    return mMass;
}

ePhysicsBodyType PhysicsDescriptor::GetPhysicsBodyType() const
{
    return mBodyType;
}

MotionModifiers PhysicsDescriptor::GetMotionModifiers() const
{
    return mMotionModifier;
}

const std::shared_ptr<CollisionShapeBase>& PhysicsDescriptor::GetShape() const
{
    return mShape;
}

size_t PhysicsDescriptor::GetId() const
{
    return mCurrentId;
}

btRigidBody* PhysicsDescriptor::GetRigidBody() const
{
    return mRigidBody;
}

btMotionState* PhysicsDescriptor::GetMotionState() const
{
    return mMotionState;
}

void PhysicsDescriptor::SetLinearVelocity(const btVector3& velocity)
{
    if (mRigidBody) {
        mRigidBody->setLinearVelocity(velocity);
    }
}

btQuaternion PhysicsDescriptor::GetRotator() const
{
    return mRotator;
}

btVector3 PhysicsDescriptor::GetTranslation() const
{
    return mTranslation;
}

btVector3 PhysicsDescriptor::GetVelocity() const
{

    return mVelocity;
}

void PhysicsDescriptor::SetTranslation(const btVector3& translation)
{
    assert(mRigidBody);
    btTransform& worldTransform = mRigidBody->getWorldTransform();
    worldTransform.setOrigin(translation);
}

void PhysicsDescriptor::SetRotator(const btQuaternion& rotator)
{
    assert(mRigidBody);
    btTransform& worldTransform = mRigidBody->getWorldTransform();
    worldTransform.setRotation(rotator);
}

void PhysicsDescriptor::SetIsCollisionEnabled(const bool isCollisionEnabled)
{
    assert(mRigidBody);
    if (mIsCollisionEnabled != isCollisionEnabled) {
        if (isCollisionEnabled) {
            mPhysicsWorld->GetWorld()->addCollisionObject(mRigidBody);
            mRigidBody->setCollisionFlags(mRigidBody->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE);
        } else {
            mPhysicsWorld->GetWorld()->removeCollisionObject(mRigidBody);
            mRigidBody->setCollisionFlags(mRigidBody->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
        }
        mIsCollisionEnabled = isCollisionEnabled;
    }
}

bool PhysicsDescriptor::GetIsCollisionEnabled() const
{
    return mIsCollisionEnabled;
}

void PhysicsDescriptor::SetOwnerComponentEngineObjectId(const int32_t ownerComponentEngineObjectId)
{
    mOwnerComponentEngineObjectId = ownerComponentEngineObjectId;
}

int32_t PhysicsDescriptor::GetOwnerComponentEngineObjectId() const
{
    return mOwnerComponentEngineObjectId;
}

void PhysicsDescriptor::SetOwnerActorEngineObjectId(const int32_t ownerActorEngineObjectId)
{
    mOwnerActorEngineObjectId = ownerActorEngineObjectId;
}

int32_t PhysicsDescriptor::GetOwnerActorEngineObjectId() const
{
    return mOwnerActorEngineObjectId;
}
} // namespace EnginePhysics