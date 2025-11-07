#include "RigidBodyController.h"

#include "Core/GameCore/Physics/CollisionTestImplementation/BulletRayCastWithFilter.h"
#include "Core/GameCore/Physics/PhysicsWorld.h"
#include "Core/UtilityCore/EngineMath.h"

namespace EnginePhysics {
RigidBodyController::RigidBodyController(
    const std::shared_ptr<PhysicsWorld>& pPhysicsWorld,
    const std::shared_ptr<CollisionShapeBase>& shape,
    const ePhysicsBodyType bodyType,
    const float mass,
    const MotionModifiers& motionModifier)
    : PhysicsDescriptor(pPhysicsWorld, shape, bodyType, mass, motionModifier)
    , mLastRayCastObjectResultId(std::numeric_limits<uint32_t>::max())
{
}

RigidBodyController::~RigidBodyController()
{
    if (ePhysicsBodyType::DYNAMIC == mBodyType) {
        Event::KinematicBodyMovedGameThreadEvent::GetInstance()->RemoveListener(GetInstanceId());
    }
}

void RigidBodyController::CleanUp()
{
    if (mRigidBody) {
        mPhysicsWorld->GetWorld()->removeRigidBody(mRigidBody);
        delete mRigidBody;
    }

    if (mMotionState) {
        delete mMotionState;
        mMotionState = nullptr;
    }
}

ePhysicsDescriptorType RigidBodyController::GetPhysicsDescriptorType() const
{
    return ePhysicsDescriptorType::RIGID_BODY_CONTROLLER;
}

std::vector<btCollisionObject*> RigidBodyController::GetCollisionObjects() const
{
    return {mRigidBody};
}

void RigidBodyController::SetMotionStateWorldTransform(const btQuaternion& quat, const btVector3& translation)
{
    btTransform worldTransform(quat, translation);
    mMotionState->setWorldTransform(worldTransform);
}

void RigidBodyController::CompletePhysicsDescriptorConstruction()
{
    btRigidBody::btRigidBodyConstructionInfo info(mMass, mMotionState, mShape->GetCollisionShape(), mInertia);
    mRigidBody = new btRigidBody(info);

    // apply motion modifiers
    mRigidBody->setLinearFactor(mMotionModifier.LinearFactor);
    mRigidBody->setAngularFactor(mMotionModifier.AngularFactor);

    switch (mBodyType) {
    case ePhysicsBodyType::DYNAMIC: {
        mRigidBody->setActivationState(DISABLE_DEACTIVATION);
        Event::KinematicBodyMovedGameThreadEvent::GetInstance()->AddListener(
            std::dynamic_pointer_cast<RigidBodyController>(shared_from_this()));
        break;
    }
    case ePhysicsBodyType::KINEMATIC:
        mRigidBody->setCollisionFlags(mRigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
        break;
    case ePhysicsBodyType::STATIC:
        assert(EngineMath::FloatsNearEqual(mMass, 0.0f));
        break;
    }

    mRigidBody->setUserPointer(static_cast<PhysicsDescriptor*>(this));

    mPhysicsWorld->GetWorld()->addRigidBody(mRigidBody);
}

bool RigidBodyController::DoRayCastDown()
{
    auto ignoreMeRayCast = BulletRayCastWithFilter({mRigidBody});
    auto& worldTransform = mRigidBody->getWorldTransform();

    const btVector3& downDir = worldTransform.getOrigin() - btVector3(0, 5.0f, 0);

    ignoreMeRayCast.RayTest(mPhysicsWorld->GetWorld(), worldTransform.getOrigin(), downDir);

    const bool bResult = ignoreMeRayCast.IsRayHitCollision();

    if (bResult) {
        if (auto collidedUserPtr = ignoreMeRayCast.GetCollisionHitObject()->getUserPointer()) {
            mLastRayCastObjectResultId = static_cast<PhysicsDescriptor*>(collidedUserPtr)->GetId();
        }
    } else {
        mLastRayCastObjectResultId = std::numeric_limits<uint32_t>::max();
    }

    return bResult;
}

void RigidBodyController::ProcessEvent(
    const KinematicBodyMovedGameThreadEvent* sender, const Event::KinematicBodyMovedGameThreadEvent::EventData_t& data)
{
    if (const auto& kinematicObjDescSp = std::get<0>(data).lock()) {
        const btVector3& offsetTranslation = Converter::glmToBullet(std::get<1>(data).Translation);

        if (DoRayCastDown()) {
            if (mLastRayCastObjectResultId == kinematicObjDescSp->GetId()) {
                // Collision
                auto& worldTransform = mRigidBody->getWorldTransform();
                const auto& offsetedTranslation = worldTransform.getOrigin() + offsetTranslation;
                worldTransform.setOrigin(offsetedTranslation);
            }
        }
    }
}

void RigidBodyController::UpdateMotionWorldTransformLocalState(bool& bIsWorldTransformDiry, const float deltaTimeSec)
{
    btTransform transform;
    mMotionState->getWorldTransform(transform);

    if (bIsWorldTransformDiry = !(isEqual(transform, mPrevTransform))) {
        mPrevTransform = transform;

        mRotator = transform.getRotation();
        mTranslation = transform.getOrigin();
        mVelocity = mRigidBody->getLinearVelocity();
    }
}
} // namespace EnginePhysics