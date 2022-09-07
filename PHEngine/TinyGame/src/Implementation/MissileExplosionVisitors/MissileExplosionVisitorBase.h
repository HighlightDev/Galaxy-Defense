#pragma once

#include <memory>

namespace EngineCore
{
    class Actor;
}

namespace Game
{
    class SpaceshipActor;
    class MissileActor;
    class SpaceObjectActor;

    class MissileExplosionVisitorBase
    {
    protected:
        std::weak_ptr<MissileActor> mOwnerWp;

    public:
        MissileExplosionVisitorBase(const std::shared_ptr<MissileActor> &ownerMissile);

        /**
         * @brief Process start explosion for spaceship
         *
         * @param spaceship spaceship actor
         * @param missileCollidedActor concrete missile actor which collided
         * @param spaceshipCollidedActor concrete spaceship actor which collided
         */
        virtual void StartExplosionForSpaceship(const std::shared_ptr<SpaceshipActor> &spaceship,
                                                const std::shared_ptr<::EngineCore::Actor> &missileCollidedActor,
                                                const std::shared_ptr<::EngineCore::Actor> &spaceshipCollidedActor) = 0;

        /**
         * @brief Process end explosion for spaceship
         *
         * @param spaceship spaceship actor
         * @param missileCollidedActor concrete missile actor which collided
         * @param spaceshipCollidedActor concrete spaceship actor which collided
         */
        virtual void EndExplosionForSpaceship(const std::shared_ptr<SpaceshipActor> &spaceship,
                                              const std::shared_ptr<::EngineCore::Actor> &missileCollidedActor,
                                              const std::shared_ptr<::EngineCore::Actor> &spaceshipCollidedActor) = 0;

        virtual void StartExplosionForSpaceObject(const std::shared_ptr<SpaceObjectActor> &spaceObject,
                                                  const std::shared_ptr<::EngineCore::Actor> &missileCollidedActor,
                                                  const std::shared_ptr<::EngineCore::Actor> &spaceshipCollidedActor) = 0;

        virtual void EndExplosionForSpaceObject(const std::shared_ptr<SpaceObjectActor> &spaceObject,
                                                const std::shared_ptr<::EngineCore::Actor> &missileCollidedActor,
                                                const std::shared_ptr<::EngineCore::Actor> &spaceshipCollidedActor) = 0;
    };
}