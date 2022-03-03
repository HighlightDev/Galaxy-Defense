#include "AiActorController.h"

namespace Game
{
    AiActorController::AiActorController(const std::shared_ptr<Actor>& actor)
        : ActorController(actor)
    {
    }

    void AiActorController::Tick(const float deltaTime)
    {
        m_movementComponent->SetDirection(glm::vec3(0.0f, 0.0f, -1.0f));
        m_movementComponent->Move();
    }

    void AiActorController::InitActorController()
    {
        ActorController::InitActorController();
    }
}