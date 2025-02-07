#pragma once

#include "Core/GameCore/Event/TEvent.h"

#include <glm/vec4.hpp>

namespace Event {
struct MouseMovedGameThreadEvent : public TEvent<
                                       MouseMovedGameThreadEvent,
                                       eEventThreadType::GAME_THREAD,
                                       SingleDataEventPolicy<glm::ivec4 /*X, Y, deltaX, deltaY*/>> {
public:
    using Event_t = TEvent<MouseMovedGameThreadEvent, eEventThreadType::GAME_THREAD, SingleDataEventPolicy<glm::ivec4>>::Event_t;

    std::string ToString() const override
    {
        return "MouseMovedGameThreadEvent";
    }
};

struct MouseMovedLuaThreadEvent : public TEvent<
                                      MouseMovedLuaThreadEvent,
                                      eEventThreadType::LUA_THREAD,
                                      SingleDataEventPolicy<glm::ivec4 /*X, Y, deltaX, deltaY*/>> {
public:
    using Event_t = TEvent<MouseMovedLuaThreadEvent, eEventThreadType::LUA_THREAD, SingleDataEventPolicy<glm::ivec4>>::Event_t;

    std::string ToString() const override
    {
        return "MouseMovedLuaThreadEvent";
    }
};

} // namespace Event
