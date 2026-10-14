#pragma once

#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"
#include "TEvent.h"

namespace Event {
class WindowSizeChangedGameThreadEvent : public TEvent<
                                             WindowSizeChangedGameThreadEvent,
                                             Event::eEventThreadType::GAME_THREAD,
                                             SingleDataEventPolicy<::Graphics::ViewPortInfo>> {
public:
    using Type_t = TEvent<
        WindowSizeChangedGameThreadEvent,
        eEventThreadType::GAME_THREAD,
        MultipleDataEventPolicy<::Graphics::ViewPortInfo>>::Type_t;

    std::string ToString() const override
    {
        return "WindowSizeChangedGameThreadEvent";
    }
};

class WindowSizeChangedLuaThreadEvent : public TEvent<
                                            WindowSizeChangedLuaThreadEvent,
                                            Event::eEventThreadType::LUA_THREAD,
                                            SingleDataEventPolicy<::Graphics::ViewPortInfo>> {
public:
    using Type_t = TEvent<
        WindowSizeChangedLuaThreadEvent,
        eEventThreadType::LUA_THREAD,
        MultipleDataEventPolicy<::Graphics::ViewPortInfo>>::Type_t;

    std::string ToString() const override
    {
        return "WindowSizeChangedLuaThreadEvent";
    }
};
} // namespace Event
