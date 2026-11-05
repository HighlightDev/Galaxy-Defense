#pragma once

#include "Core/GameCore/Event/Policy/Policies.h"
#include "Core/GameCore/Event/TEvent.h"
#include "Game/EditModeTypeEnum.h"

#include <string>

using namespace Game;

namespace Event {
class LuaLevelFilesListChangedEvent : public TEvent<
                                          LuaLevelFilesListChangedEvent,
                                          eEventThreadType::LUA_THREAD,
                                          SingleDataEventPolicy<std::vector<std::string>>> {
public:
    using Type_t
        = TEvent<LuaLevelFilesListChangedEvent, eEventThreadType::LUA_THREAD, SingleDataEventPolicy<std::vector<std::string>>>::
            Type_t;

    std::string ToString() const override
    {
        return "LuaLevelFilesListChangedEvent";
    }
};

} // namespace Event