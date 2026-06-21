#pragma once

#include "Core/GameCore/Event/TEvent.h"

namespace Event {
struct PlaySpeedGameThreadEvent : public TEvent<PlaySpeedGameThreadEvent, eEventThreadType::GAME_THREAD, SingleDataEventPolicy<float>> {
public:
    using Type_t = TEvent<PlaySpeedGameThreadEvent, eEventThreadType::GAME_THREAD, SingleDataEventPolicy<float>>::Type_t;

    std::string ToString() const override
    {
        return "PlaySpeedGameThreadEvent";
    }
};

} // namespace Event
