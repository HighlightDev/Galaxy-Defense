#include "GravityModifier.h"
#include "Implementation/Actors/SpaceshipActor.h"
#include "Implementation/Actors/MissileActor.h"

namespace Game
{
    GravityModifiable::GravityModifiable(const std::weak_ptr<SpaceshipActor> &owner,
                                         const std::weak_ptr<MissileActor> &missile,
                                         const glm::vec3 &gravityCenterPosition)
        : mOwnerWp(owner),
          mMissileWp(missile),
          mGravityCenterPosition(gravityCenterPosition),
          mGravityPower(1.0f)
    {
    }

    eModifierType GravityModifiable::GetModifierType() const
    {
        return eModifierType::Gravity;
    }

    uint64_t GravityModifiable::CreatorObjectId() const
    {
        const auto& ownerSp = mOwnerWp.lock();
        assert(ownerSp);
        return ownerSp->GetObjectId();
    }

    void GravityModifiable::Tick(const float deltaTime)
    {
        const auto &spaceshipSp = mOwnerWp.lock();
        const auto &missileSp = mMissileWp.lock();

        if (spaceshipSp && missileSp)
        {
            const auto spaceshipPosition = spaceshipSp->GetRootComponent()->GetTranslation();
            const auto toGravityCenterVec = mGravityCenterPosition - spaceshipPosition;
            const auto vecLength = glm::length(toGravityCenterVec);
            if (vecLength > 0.001f) // check if length of vector is not zero otherwise normalized vector will be NaN 
            {
                const auto nToGravityCenter = toGravityCenterVec / vecLength;
                spaceshipSp->GetMovementComponent()->Move(nToGravityCenter * mGravityPower, deltaTime);
            }
        }
    }

    void GravityModifiable::SetGravityPower(const float gravityPower)
    {
        mGravityPower = gravityPower;
    }
}