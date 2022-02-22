#pragma once

#include "Actor.h"
#include "Core/GameCore/ITickable.h"
#include "Core/GameCore/Serialize/ISerializable.h"
#include "Core/GameCore/Components/MovementComponent.h"

namespace EngineCore
{
    class ActorController
        : public ITickable
        , public ISerializable
    {
    protected:
        std::shared_ptr<Actor> m_playerActor;

        std::shared_ptr<MovementComponent> m_movementComponent;

    public:
        ActorController(std::shared_ptr<Actor> playerActor);

        virtual ~ActorController();

        virtual void Tick(float deltaTime) = 0;

        std::shared_ptr<Actor> GetBindedActor() const;

        std::string GetBindedActorName() const;

        virtual void CollectDataForSerialization(SerializeDataContainer& dataContainer) override;

    protected:
        virtual void InitPlayerController() = 0;
    };

}
