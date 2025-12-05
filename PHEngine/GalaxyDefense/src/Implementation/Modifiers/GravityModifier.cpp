#include "GravityModifier.h"

#include "Implementation/Actors/MissileActor.h"
#include "Implementation/Actors/SpaceshipActor.h"

namespace Game {
GravityModifier::GravityModifier(
    const std::weak_ptr<SpaceshipActor>& owner,
    const std::weak_ptr<MissileActor>& missile,
    const glm::vec3& gravityCenterPosition)
    : mOwnerWp(owner)
    , mMissileWp(missile)
    , mGravityCenterPosition(gravityCenterPosition)
    , mGravityPower(1.0f)
{
}

eModifierType GravityModifier::GetModifierType() const
{
    return eModifierType::Gravity;
}

int32_t GravityModifier::CreatorObjectId() const
{
    const auto& ownerSp = mOwnerWp.lock();
    ext_assert(ownerSp, "GravityModifier owner spaceship pointer is null");
    return ownerSp->GetObjectId();
}

void GravityModifier::Tick(const float deltaTimeSec)
{
    const auto& spaceshipSp = mOwnerWp.lock();
    const auto& missileSp = mMissileWp.lock();

    if (spaceshipSp && missileSp) {
        const auto spaceshipPosition = spaceshipSp->GetRootComponent()->GetTranslation();
        const auto toGravityCenterVec = mGravityCenterPosition - spaceshipPosition;
        const auto vecLength = glm::length(toGravityCenterVec);
        if (vecLength > 0.001f) // check if length of vector is not zero otherwise normalized vector will be NaN
        {
            const auto nToGravityCenter = toGravityCenterVec / vecLength;
            spaceshipSp->GetMovementComponent()->Move(nToGravityCenter * mGravityPower, deltaTimeSec);
        }
    }
}

void GravityModifier::OnPreRemoved()
{
}

bool GravityModifier::IsExpired() const
{
    return false;
}

void GravityModifier::SetGravityPower(const float gravityPower)
{
    mGravityPower = gravityPower;
}
} // namespace Game