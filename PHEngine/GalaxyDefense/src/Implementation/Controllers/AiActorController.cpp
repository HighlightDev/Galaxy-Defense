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
        if (const auto &actorSp = m_actorWp.lock())
        {
            if (!actorSp->IsEnabled())
                return;

            if (const auto &moveCompSp = m_movementComponentWp.lock())
            {
                moveCompSp->Move(deltaTime);
            }
        }
    }
}