#pragma once

#include "MissileExplosionVisitorBase.h"

#include <glm/vec3.hpp>

namespace EngineCore {
class Actor;
}

namespace Game {
class MissileActor;
class SpaceshipActor;
class SpaceObjectActor;

class FreezingExplosionVisitor : public MissileExplosionVisitorBase {
public:
    FreezingExplosionVisitor(const std::shared_ptr<MissileActor>& ownerMissile);

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

private:
    void ProcessProjectileWithSpaceshipsCollision(const glm::vec3& explosionCenterPosition);

    void AddOrResetFreezingModifier(const std::shared_ptr<SpaceshipActor>& spaceship);
};
} // namespace Game