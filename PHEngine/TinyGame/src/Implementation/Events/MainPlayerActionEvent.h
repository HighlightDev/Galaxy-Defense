#pragma once

#include "Core/GameCore/Event/Policy/Policies.h"
#include "Core/GameCore/Event/TEvent.h"
#include "Implementation/MainPlayerActionEnum.h"

using namespace Game;

namespace Event {
class MainPlayerActionEvent : public TEvent<eEventThreadType::GAME_THREAD, SingleDataEventPolicy<eMainPlayerActionEnum>> {
public:
    using Event_t = TEvent<eEventThreadType::GAME_THREAD, SingleDataEventPolicy<eMainPlayerActionEnum>>::Event_t;

    std::string ToString() const override
    {
        return "GameThreadMainPlayerActionEvent";
    }
};

} // namespace Event