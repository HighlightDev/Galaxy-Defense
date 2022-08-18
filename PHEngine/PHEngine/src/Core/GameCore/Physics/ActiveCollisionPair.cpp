#include "ActiveCollisionPair.h"

namespace EnginePhysics
{
    ActiveCollisionPair::ActiveCollisionPair(const PhysicsDescriptor *collisionBody1, const PhysicsDescriptor *collisionBody2)
        : mFirstCollisionBody(collisionBody1),
          mSecondCollisionBody(collisionBody2),
          mFirstCollisionBodyId(collisionBody1->GetId()),
          mSecondCollisionBodyId(collisionBody2->GetId())
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

    const PhysicsDescriptor *ActiveCollisionPair::GetFirstCollisionBody() const
    {
        return mFirstCollisionBody;
    }

    const PhysicsDescriptor *ActiveCollisionPair::GetSecondCollisionBody() const
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

        if (mActiveCollisionLefitime >= mActiveCollisionTimeout)
        {
            mIsCollisionExpired = true;
            mActiveCollisionLefitime = 0.0f;
        }
    }

    bool ActiveCollisionPair::IsCollisionExpired() const
    {
        return mIsCollisionExpired;
    }
}