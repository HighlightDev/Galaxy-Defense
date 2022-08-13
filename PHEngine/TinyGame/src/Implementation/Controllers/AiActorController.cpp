#include "AiActorController.h"
#include "Core/UtilityCore/EngineMath.h"

#include <vector>
#include <utility>
#include <algorithm>

using namespace EngineMath;

namespace Game
{
    AiActorController::AiActorController(const std::shared_ptr<Actor> &actor)
        : ActorController(actor)
    {
    }

    void AiActorController::Tick(const float deltaTime)
    {
        if (!m_actor->IsEnabled())
            return;
        
        m_movementComponent->Move(deltaTime);
    }

    void AiActorController::InitActorController()
    {
        ActorController::InitActorController();
    }
}