#include "HumanoidPlayerController.h"

namespace EngineCore {
ActorController::ActorController(const std::shared_ptr<Actor>& actor)
    : m_actorWp(actor)
    , m_movementComponentWp()
{
}

ActorController::~ActorController()
{
}

void ActorController::Initialize()
{
    const auto& actorSp = m_actorWp.lock();
    ext_assert(actorSp, "Actor pointer is null in ActorController::Initialize");
    ext_assert(actorSp->GetMovementComponent(), "MovementComponent is null in ActorController::Initialize");
    m_movementComponentWp = actorSp->GetMovementComponent();
}

void ActorController::CleanUp()
{
}

std::weak_ptr<Actor> ActorController::GetBindedActor() const
{
    return m_actorWp;
}

std::string ActorController::GetBindedActorName() const
{
    if (const auto& actorSp = m_actorWp.lock()) {
        return actorSp->GetName();
    }
    return "";
}

void ActorController::Tick(const float deltaTimeSec)
{
}

void ActorController::UnpausableTick(const float deltaTimeSec)
{
}

} // namespace EngineCore