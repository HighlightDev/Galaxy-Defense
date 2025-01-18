#include "WindowSizeChangedEvent.h"

namespace Event
{
    template class TEvent<WindowSizeChangedGameThreadEvent, Event::eEventThreadType::GAME_THREAD, SingleDataEventPolicy<::Graphics::ViewPortInfo>>;

    template class TEvent<WindowSizeChangedLuaThreadEvent, Event::eEventThreadType::LUA_THREAD, SingleDataEventPolicy<::Graphics::ViewPortInfo>>;
}
