#pragma once

#include "Actor.h"
#include "Core/GameCore/ITickable.h"

namespace Game
{
    class ActorController
        : public ITickable
    {
    protected:
        std::shared_ptr<Actor> m_playerActor;

    public:
        ActorController(std::shared_ptr<Actor> playerActor);

        virtual ~ActorController();

        virtual void Tick(float deltaTime) = 0;

        std::shared_ptr<Actor> GetBindedActor() const;

    protected:
        virtual void SetPlayerActor(std::shared_ptr<Actor> playerActor) = 0;
    };

}
