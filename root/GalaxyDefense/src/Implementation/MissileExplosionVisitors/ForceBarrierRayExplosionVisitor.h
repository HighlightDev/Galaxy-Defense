#pragma once

#include "MissileExplosionVisitorBase.h"

namespace EngineCore {
class Actor;
}

namespace Game {
class MissileActor;
class SpaceshipActor;
class SpaceObjectActor;
class BarrierActor;

class ForceBarrierRayExplosionVisitor : public MissileExplosionVisitorBase {
public:
    ForceBarrierRayExplosionVisitor(const std::shared_ptr<MissileActor>& ownerMissile);

    virtual void StartExplosionForSpaceship(
        const std::shared_ptr<SpaceshipActor>& spaceship,
        const std::shared_ptr<::EngineCore::Actor>& missileCollidedActor) override;

    virtual void EndExplosionForSpaceship(
        const std::shared_ptr<SpaceshipActor>& spaceship,
        const std::shared_ptr<::EngineCore::Actor>& missileCollidedActor) override;

    virtual void StartExplosionForSpaceObject(
        const std::shared_ptr<SpaceObjectActor>& spaceObject,
        const std::shared_ptr<::EngineCore::Actor>& missileCollidedActor) override;

    virtual void EndExplosionForSpaceObject(
        const std::shared_ptr<SpaceObjectActor>& spaceObject,
        const std::shared_ptr<::EngineCore::Actor>& missileCollidedActor) override;

    virtual void StartExplosionForBarrier(
        const std::shared_ptr<BarrierActor>& barrier, const std::shared_ptr<::EngineCore::Actor>& missileCollidedActor) override;

    virtual void EndExplosionForBarrier(
        const std::shared_ptr<BarrierActor>& barrier, const std::shared_ptr<::EngineCore::Actor>& missileCollidedActor) override;

private:
    void AddForceBarrierModifier(const std::shared_ptr<BarrierActor>& barrier);
};
} // namespace Game
