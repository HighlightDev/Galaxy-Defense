#include "AiActorController.h"
#include "Core/UtilityCore/EngineMath.h"

#include <vector>
#include <utility>
#include <algorithm>

using namespace EngineMath;

namespace Game
{
    AiActorController::AiActorController(const std::shared_ptr<Actor> &actor)
        : ActorController(actor), mCurrentState("s_idle")
    {
    }

    void AiActorController::Tick(const float deltaTime)
    {
        if (!m_actor->IsEnabled())
            return;
            
        m_movementComponent->Move();

        const auto &moveDirection = m_movementComponent->GetDirection();
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
            m_actor->ChangeTweenState(actualDirectionName);
            mCurrentState = actualDirectionName;
        }
    }

    void AiActorController::InitActorController()
    {
        ActorController::InitActorController();
    }
}