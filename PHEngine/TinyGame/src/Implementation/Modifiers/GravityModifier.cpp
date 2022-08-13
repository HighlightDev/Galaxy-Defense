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

    void GravityModifiable::Tick(const float deltaTime)
    {
        const auto &spaceshipSp = mOwnerWp.lock();
        const auto &missileSp = mMissileWp.lock();

        if (spaceshipSp && missileSp)
        {
            const auto spaceshipPosition = spaceshipSp->GetBaseRootComponent()->GetTranslation();
            const auto nToGravityCenter = glm::normalize(mGravityCenterPosition - spaceshipPosition);
            spaceshipSp->GetMovementComponent()->Move(nToGravityCenter * mGravityPower, deltaTime);
        }
    }

    void GravityModifiable::SetGravityPower(const float gravityPower)
    {
        mGravityPower = gravityPower;
    }
}