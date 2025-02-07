#pragma once

#include "Policy/Policies.h"
#include "TEvent.h"

#include <string>

namespace Event {
class LoadLevelGameThreadEvent
    : public TEvent<LoadLevelGameThreadEvent, eEventThreadType::GAME_THREAD, MultipleDataEventPolicy<std::string>> {
public:
    using Event = TEvent<LoadLevelGameThreadEvent, eEventThreadType::GAME_THREAD, MultipleDataEventPolicy<std::string>>::Event_t;

    std::string ToString() const override
    {
        return "LoadLevelGameThreadEvent";
    }
};

} // namespace Event