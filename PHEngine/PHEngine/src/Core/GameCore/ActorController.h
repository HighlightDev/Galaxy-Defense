#pragma once

#include "Actor.h"
#include "Core/GameCore/Components/MovementComponent.h"
#include "Core/GameCore/ITickable.h"
#include "Core/GameCore/Serialize/ISerializable.h"

#include <memory>

namespace EngineCore {
class ActorController : public ITickable, public ISerializable, public std::enable_shared_from_this<ActorController> {
protected:
    std::weak_ptr<Actor> m_actorWp;

    std::weak_ptr<MovementComponent> m_movementComponentWp;

public:
    ActorController(const std::shared_ptr<Actor>& actor);

    virtual ~ActorController();

    virtual void CleanUp();

    virtual void Initialize();

    void Tick(const float deltaTime) override
    {
    }

    void UnpausableTick(const float deltaTime) override { };

    std::weak_ptr<Actor> GetBindedActor() const;

    std::string GetBindedActorName() const;

    void CollectDataForSerialization(SerializeDataContainer& dataContainer) override;
};

} // namespace EngineCore
