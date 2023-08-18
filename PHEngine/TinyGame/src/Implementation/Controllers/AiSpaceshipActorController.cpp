#include "AiSpaceshipActorController.h"
#include "Core/UtilityCore/EngineMath.h"

#include <vector>
#include <utility>
#include <algorithm>

using namespace EngineMath;

namespace Game
{
    AiSpaceshipActorController::AiSpaceshipActorController(const std::shared_ptr<Actor> &actor)
        : AiActorController(actor),
          mCurrentState("s_idle")
    {
    }

    void AiSpaceshipActorController::Tick(const float deltaTime)
    {
        AiActorController::Tick(deltaTime);

        if (const auto &actorSp = m_actorWp.lock())
        {
            if (const auto &movementComponentSp = m_movementComponentWp.lock())
            {
                const auto &moveDirection = movementComponentSp->GetDirection();
                const float dotLeft = glm::dot(moveDirection, -AXIS_RIGHT);
                const float dotRight = glm::dot(moveDirection, AXIS_RIGHT);
                const float dotForward = glm::dot(moveDirection, AXIS_FORWARD);
                const float dotBack = glm::dot(moveDirection, -AXIS_FORWARD);

                std::vector<std::pair<std::string, float>> dotVector = {
                    std::make_pair("s_fly_left", dotLeft),
                    std::make_pair("s_fly_right", dotRight),
                    std::make_pair("s_fly_forward", dotForward),
                    std::make_pair("s_fly_back", dotBack)};

                const auto &greatestDot = std::max_element(dotVector.begin(), dotVector.end(),
                                                           [](const auto &left, const auto &right)
                                                           {
                                                               return left.second < right.second;
                                                           });

                const auto &actualDirectionName = greatestDot->first;
                if (actualDirectionName != mCurrentState)
                {
                    actorSp->ChangeTweenerState("SpaceshipMovement", actualDirectionName);
                    mCurrentState = actualDirectionName;
                }
            }
        }
    }

    void AiSpaceshipActorController::InitActorController()
    {
        AiActorController::InitActorController();
    }
}