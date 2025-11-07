#include "AiActorController.h"

#include "Core/UtilityCore/EngineMath.h"

#include <algorithm>
#include <utility>
#include <vector>

using namespace EngineMath;

namespace Game {
AiActorController::AiActorController(const std::shared_ptr<Actor>& actor)
    : ActorController(actor)
{
}

void AiActorController::Tick(const float deltaTimeSec)
{
    if (const auto& actorSp = m_actorWp.lock()) {
        if (!actorSp->IsEnabled())
            return;

        if (const auto& moveCompSp = m_movementComponentWp.lock()) {
            if (moveCompSp->IsEnabled()) {
                moveCompSp->Move(deltaTimeSec);
            }
        }
    }
}
} // namespace Game