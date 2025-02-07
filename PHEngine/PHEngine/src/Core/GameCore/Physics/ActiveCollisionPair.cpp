#include "ActiveCollisionPair.h"

namespace EnginePhysics {
ActiveCollisionPair::ActiveCollisionPair(
    const std::shared_ptr<PhysicsDescriptor>& collisionBody1, const std::shared_ptr<PhysicsDescriptor>& collisionBody2)
    : mFirstCollisionBody(collisionBody1)
    , mSecondCollisionBody(collisionBody2)
    , mFirstCollisionBodyId(collisionBody1->GetId())
    , mSecondCollisionBodyId(collisionBody2->GetId())
    , mFirstCollisionBodyType(collisionBody1->GetPhysicsBodyType())
    , mSecondCollisionBodyType(collisionBody2->GetPhysicsBodyType())
    , mFirstCollisionBodyOwnerActorObjectId(collisionBody1->GetOwnerActorEngineObjectId())
    , mSecondCollisionBodyOwnerActorObjectId(collisionBody2->GetOwnerActorEngineObjectId())
{
}

uint32_t ActiveCollisionPair::GetFirstCollisionBodyId() const
{
    return mFirstCollisionBodyId;
}

uint32_t ActiveCollisionPair::GetSecondCollisionBodyId() const
{
    return mSecondCollisionBodyId;
}

ePhysicsBodyType ActiveCollisionPair::GetFirstCollisionBodyType() const
{
    return mFirstCollisionBodyType;
}

ePhysicsBodyType ActiveCollisionPair::GetSecondCollisionBodyType() const
{
    return mSecondCollisionBodyType;
}

int32_t ActiveCollisionPair::GetFirstCollisionBodyOwnerActorObjectId() const
{
    return mFirstCollisionBodyOwnerActorObjectId;
}

int32_t ActiveCollisionPair::GetSecondCollisionBodyOwnerActorObjectId() const
{
    return mSecondCollisionBodyOwnerActorObjectId;
}

std::weak_ptr<PhysicsDescriptor> ActiveCollisionPair::GetFirstCollisionBody() const
{
    return mFirstCollisionBody;
}

std::weak_ptr<PhysicsDescriptor> ActiveCollisionPair::GetSecondCollisionBody() const
{
    return mSecondCollisionBody;
}

void ActiveCollisionPair::ReloadActiveCollisionLifetime()
{
    mActiveCollisionLefitime = 0.0f;
}

float ActiveCollisionPair::GetActiveCollisionLifetime() const
{
    return mActiveCollisionLefitime;
}

float ActiveCollisionPair::GetActiveCollisionTimeout() const
{
    return mActiveCollisionTimeout;
}

void ActiveCollisionPair::SetActiveCollisionLifetime(const float lifetime)
{
    mActiveCollisionLefitime = lifetime;
}

void ActiveCollisionPair::SetActiveCollisionTimeout(const float timeout)
{
    mActiveCollisionTimeout = timeout;
}

void ActiveCollisionPair::Tick(const float deltaTime)
{
    mActiveCollisionLefitime += deltaTime;

    if (mActiveCollisionLefitime >= mActiveCollisionTimeout
        || (mFirstCollisionBody.expired() || mSecondCollisionBody.expired())) {
        mIsCollisionExpired = true;
        mActiveCollisionLefitime = 0.0f;
    }
}

bool ActiveCollisionPair::IsCollisionExpired() const
{
    return mIsCollisionExpired;
}
} // namespace EnginePhysics