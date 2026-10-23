#include "LootDropMovementComponent.h"

#include "Core/GameCore/Components/ComponentData/MovementComponentData.h"

#include <glm/gtc/constants.hpp>

#include <cmath>

namespace Game {

LootDropMovementComponent::LootDropMovementComponent(const std::shared_ptr<MovementComponentData>& movementComponentData)
    : NoPhysicsMovementComponent(movementComponentData)
{
    mCurrentSpeed = 3.0f;
}

void LootDropMovementComponent::Move(const float deltaTimeSec)
{
    if (mPhase == eLootDropPhase::FALLING) {
        if (const auto& rootSp = m_actorRootComponent.lock()) {
            const float step = mCurrentSpeed * deltaTimeSec;
            const float remaining = mFallDistance - mFallenSoFar;

            if (step >= remaining) {
                // Reach the target — snap and start bounce
                rootSp->AddTranslation(glm::vec3(0.0f, -remaining, 0.0f));
                mFallenSoFar = mFallDistance;
                mBounceBasePosition = rootSp->GetTranslation();
                mPhase = eLootDropPhase::BOUNCING;
            } else {
                rootSp->AddTranslation(glm::vec3(0.0f, -step, 0.0f));
                mFallenSoFar += step;
            }
        }
    } else if (mPhase == eLootDropPhase::BOUNCING) {
        mBounceTimeElapsed += deltaTimeSec;

        if (mBounceTimeElapsed >= mBounceDuration) {
            // Snap to base and finish
            if (const auto& rootSp = m_actorRootComponent.lock()) {
                rootSp->SetTranslation(mBounceBasePosition);
            }
            mPhase = eLootDropPhase::DONE;
        } else {
            // sine arc: phase goes from 0 to pi over mBounceDuration
            const float angle = (mBounceTimeElapsed * mCurrentSpeed * 0.25f / mBounceDuration) * glm::pi<float>();
            const float bounceOffset = mBounceAmplitude * std::sin(angle);
            if (const auto& rootSp = m_actorRootComponent.lock()) {
                auto pos = mBounceBasePosition;
                pos.y += bounceOffset;
                rootSp->SetTranslation(pos);
            }
        }
    }
    // eLootDropPhase::DONE — no movement
}

void LootDropMovementComponent::SetFallDistance(const float dist)
{
    mFallDistance = dist;
}

float LootDropMovementComponent::GetFallDistance() const
{
    return mFallDistance;
}

void LootDropMovementComponent::SetBounceAmplitude(const float amplitude)
{
    mBounceAmplitude = amplitude;
}

float LootDropMovementComponent::GetBounceAmplitude() const
{
    return mBounceAmplitude;
}

void LootDropMovementComponent::SetBounceDuration(const float duration)
{
    mBounceDuration = duration;
}

float LootDropMovementComponent::GetBounceDuration() const
{
    return mBounceDuration;
}

bool LootDropMovementComponent::IsDone() const
{
    return mPhase == eLootDropPhase::DONE;
}

void LootDropMovementComponent::Reset()
{
    mFallenSoFar = 0.0f;
    mBounceTimeElapsed = 0.0f;
    mBounceBasePosition = glm::vec3(0.0f);
    mPhase = eLootDropPhase::FALLING;
}

} // namespace Game
