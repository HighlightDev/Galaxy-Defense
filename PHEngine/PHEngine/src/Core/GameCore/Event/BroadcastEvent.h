#pragma once

#include "Policy/Policies.h"
#include "TEvent.h"

#include <string>

using namespace EngineCore;

namespace Event {
class BroadcastGameThreadEvent : public TEvent<
                                     BroadcastGameThreadEvent,
                                     eEventThreadType::GAME_THREAD,
                                     MultipleDataEventPolicy<std::string /*event header*/, std::string /*json parameters*/>> {
public:
    using Type_t = TEvent<
        BroadcastGameThreadEvent,
        eEventThreadType::GAME_THREAD,
        MultipleDataEventPolicy<std::string /*event header*/, std::string /*json parameters*/>>::Type_t;

    BroadcastGameThreadEvent()
        : Type_t(true)
    {
    }

    std::string ToString() const override
    {
        return "BroadcastGameThreadEvent";
    }
};

class BroadcastLuaThreadEvent : public TEvent<
                                    BroadcastLuaThreadEvent,
                                    eEventThreadType::LUA_THREAD,
                                    MultipleDataEventPolicy<std::string /*event header*/, std::string /*json parameters*/>> {
public:
    using Type_t = TEvent<
        BroadcastLuaThreadEvent,
        eEventThreadType::LUA_THREAD,
        MultipleDataEventPolicy<std::string /*event header*/, std::string /*json parameters*/>>::Type_t;

    std::string ToString() const override
    {
        return "BroadcastLuaThreadEvent";
    }
};

} // namespace Event