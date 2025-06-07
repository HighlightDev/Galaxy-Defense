#pragma once

#include "Policy/Policies.h"
#include "TEvent.h"

#include <string>

using namespace EngineCore;

namespace Event {
enum class eSystemSettingsEventType { SOUND_SETTINGS_CHANGED, MUSIC_SETTINGS_CHANGED };

class GeneralSystemSettingsChangedGameThreadEvent
    : public TEvent<
          GeneralSystemSettingsChangedGameThreadEvent,
          eEventThreadType::GAME_THREAD,
          MultipleDataEventPolicy<eSystemSettingsEventType /*event type*/, std::string /*json parameters*/>> {
public:
    using Event_t = TEvent<
        GeneralSystemSettingsChangedGameThreadEvent,
        eEventThreadType::GAME_THREAD,
        MultipleDataEventPolicy<eSystemSettingsEventType /*event header*/, std::string /*json parameters*/>>::Event_t;

    std::string ToString() const override
    {
        return "GeneralSystemSettingsChangedGameThreadEvent";
    }
};

class GeneralSystemSettingsChangedLuaThreadEvent : public TEvent < GeneralSystemSettingsChangedLuaThreadEvent,
    eEventThreadType::LUA_THREAD, MultipleDataEventPolicy<eSystemSettingsEventType /*event type*/,
    std::string /*json parameters*/ >> {
public:
    using Event_t = TEvent<
        GeneralSystemSettingsChangedLuaThreadEvent,
        eEventThreadType::LUA_THREAD,
        MultipleDataEventPolicy<eSystemSettingsEventType /*event type*/, std::string /*json parameters*/>>::Event_t;

    std::string ToString() const override
    {
        return "GeneralSystemSettingsChangedLuaThreadEvent";
    }
};

} // namespace Event