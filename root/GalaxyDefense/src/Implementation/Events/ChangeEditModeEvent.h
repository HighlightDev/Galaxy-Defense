#pragma once

#include "Core/GameCore/Event/Policy/Policies.h"
#include "Core/GameCore/Event/TEvent.h"
#include "Implementation/EditModeTypeEnum.h"

#include <string>

using namespace Game;

namespace Event {
class ChangeEditModeEvent
    : public TEvent<ChangeEditModeEvent, eEventThreadType::GAME_THREAD, SingleDataEventPolicy<eEditModeType>> {
public:
    using Type_t = TEvent<ChangeEditModeEvent, eEventThreadType::GAME_THREAD, SingleDataEventPolicy<eEditModeType>>::Type_t;

    std::string ToString() const override
    {
        return "ChangeEditModeEvent";
    }
};

} // namespace Event