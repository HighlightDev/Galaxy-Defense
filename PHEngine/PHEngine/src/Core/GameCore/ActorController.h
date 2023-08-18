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
        std::weak_ptr<Actor> m_actorWp;

        std::weak_ptr<MovementComponent> m_movementComponentWp;

    public:
        ActorController(const std::shared_ptr<Actor>& actor);

        virtual ~ActorController();

        virtual void CleanUp();

        virtual void Tick(float deltaTime) = 0;

        void UnpausableTick(const float deltaTime) override {};

        std::weak_ptr<Actor> GetBindedActor() const;

        std::string GetBindedActorName() const;

        void CollectDataForSerialization(SerializeDataContainer& dataContainer) override;

        virtual void InitActorController();
    };

}
