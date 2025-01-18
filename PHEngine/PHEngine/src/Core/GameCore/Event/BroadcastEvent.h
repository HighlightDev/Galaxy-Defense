#pragma once

#include "TEvent.h"
#include "Policy/Policies.h"

#include <string>

using namespace EngineCore;

namespace Event
{
    class BroadcastGameThreadEvent
        : public TEvent<BroadcastGameThreadEvent, eEventThreadType::GAME_THREAD, MultipleDataEventPolicy<std::string /*event header*/, std::string /*json parameters*/>>
    {
    public:
        using Event_t = TEvent<BroadcastGameThreadEvent, eEventThreadType::GAME_THREAD, MultipleDataEventPolicy<std::string /*event header*/, std::string /*json parameters*/>>::Event_t;

        std::string ToString() const override
        {
            return "BroadcastGameThreadEvent";
        }
    };

    class BroadcastLuaThreadEvent
        : public TEvent<BroadcastLuaThreadEvent, eEventThreadType::LUA_THREAD, MultipleDataEventPolicy<std::string /*event header*/, std::string /*json parameters*/>>
    {
    public:
        using Event_t = TEvent<BroadcastLuaThreadEvent, eEventThreadType::LUA_THREAD, MultipleDataEventPolicy<std::string /*event header*/, std::string /*json parameters*/>>::Event_t;

        std::string ToString() const override
        {
            return "BroadcastLuaThreadEvent";
        }
    };

}