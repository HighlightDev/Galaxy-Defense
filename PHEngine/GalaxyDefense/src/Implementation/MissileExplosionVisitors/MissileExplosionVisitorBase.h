#pragma once

#include <memory>

namespace EngineCore {
class Actor;
}

namespace Game {
class SpaceshipActor;
class MissileActor;
class SpaceObjectActor;
class BarrierActor;

class MissileExplosionVisitorBase {
protected:
    std::weak_ptr<MissileActor> mOwnerWp;

public:
    MissileExplosionVisitorBase(const std::shared_ptr<MissileActor>& ownerMissile);

    /**
     * @brief Process start explosion for spaceship
     *
     * @param spaceship spaceship actor
     * @param missileCollidedActor concrete missile actor which collided
     */
    virtual void StartExplosionForSpaceship(
        const std::shared_ptr<SpaceshipActor>& spaceship, const std::shared_ptr<::EngineCore::Actor>& missileCollidedActor)
        = 0;

    /**
     * @brief Process end explosion for spaceship
     *
     * @param spaceship spaceship actor
     * @param missileCollidedActor concrete missile actor which collided
     */
    virtual void EndExplosionForSpaceship(
        const std::shared_ptr<SpaceshipActor>& spaceship, const std::shared_ptr<::EngineCore::Actor>& missileCollidedActor)
        = 0;

    virtual void StartExplosionForSpaceObject(
        const std::shared_ptr<SpaceObjectActor>& spaceObject, const std::shared_ptr<::EngineCore::Actor>& missileCollidedActor)
        = 0;

    virtual void EndExplosionForSpaceObject(
        const std::shared_ptr<SpaceObjectActor>& spaceObject, const std::shared_ptr<::EngineCore::Actor>& missileCollidedActor)
        = 0;

    virtual void StartExplosionForBarrier(
        const std::shared_ptr<BarrierActor>& barrier, const std::shared_ptr<::EngineCore::Actor>& missileCollidedActor)
        = 0;

    virtual void EndExplosionForBarrier(
        const std::shared_ptr<BarrierActor>& barrier, const std::shared_ptr<::EngineCore::Actor>& missileCollidedActor)
        = 0;
};
} // namespace Game