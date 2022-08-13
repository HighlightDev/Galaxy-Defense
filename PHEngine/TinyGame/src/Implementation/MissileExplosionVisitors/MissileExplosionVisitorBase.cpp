#include "MissileExplosionVisitorBase.h"
#include "Implementation/Actors/MissileActor.h"

namespace Game
{
    MissileExplosionVisitorBase::MissileExplosionVisitorBase(const std::shared_ptr<MissileActor>& ownerMissile)
        : mOwnerWp(ownerMissile)
    {
    }
}