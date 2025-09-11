#pragma once

#include "Policy/Policies.h"
#include "TEvent.h"

#include <string>

namespace Event {
class RestartLevelGameThreadEvent : public TEvent<RestartLevelGameThreadEvent, eEventThreadType::GAME_THREAD, NoDataEventPolicy> {
public:
    using Event = TEvent<RestartLevelGameThreadEvent, eEventThreadType::GAME_THREAD, NoDataEventPolicy>::Type_t;

    std::string ToString() const override
    {
        return "RestartLevelGameThreadEvent";
    }
};

} // namespace Event