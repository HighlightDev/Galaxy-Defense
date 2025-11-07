#pragma once

#include "Actor.h"
#include "Core/GameCore/Components/MovementComponent.h"
#include "Core/GameCore/ITickable.h"

#include <memory>

namespace EngineCore {
class ActorController : public ITickable, public std::enable_shared_from_this<ActorController> {
protected:
    std::weak_ptr<Actor> m_actorWp;

    std::weak_ptr<MovementComponent> m_movementComponentWp;

public:
    ActorController(const std::shared_ptr<Actor>& actor);

    virtual ~ActorController();

    virtual void CleanUp();

    virtual void Initialize();

    void Tick(const float deltaTimeSec) override;

    void UnpausableTick(const float deltaTimeSec) override;

    std::weak_ptr<Actor> GetBindedActor() const;

    std::string GetBindedActorName() const;
};

} // namespace EngineCore
