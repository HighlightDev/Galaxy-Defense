#include "StateTransition.h"

#include "Core/CommonCore/Assertion.h"

namespace EngineCore {
StateTransition::StateTransition(
    std::shared_ptr<State> stateFrom, std::shared_ptr<State> stateDestination, float transitionDuration)
    : StateFrom(stateFrom)
    , StateDestination(stateDestination)
    , TransitionDuration(transitionDuration)
{
    ext_assert(stateFrom != nullptr, "StateTransition::StateTransition: stateFrom is null");
    ext_assert(stateDestination != nullptr, "StateTransition::StateTransition: stateDestination is null");
}
} // namespace EngineCore