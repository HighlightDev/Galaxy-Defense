#pragma once

#include "Core/GameCore/Event/TEvent.h"

namespace Event {
struct ExitGameThreadEvent : public TEvent<ExitGameThreadEvent, eEventThreadType::GAME_THREAD, NoDataEventPolicy> {
public:
    using Type_t = TEvent<ExitGameThreadEvent, eEventThreadType::GAME_THREAD, NoDataEventPolicy>::Type_t;

    std::string ToString() const override
    {
        return "ExitGameThreadEvent";
    }
};

} // namespace Event
