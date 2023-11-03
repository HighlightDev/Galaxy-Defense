#pragma once

#include "TEvent.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"

namespace Event
{
    class WindowSizeChangedEvent
        : public TEvent<Event::eEventThreadType::GAME_THREAD, SingleDataEventPolicy<::Graphics::ViewPortInfo>>
    {
    public:
        using Event_t = TEvent<eEventThreadType::GAME_THREAD, MultipleDataEventPolicy<::Graphics::ViewPortInfo>>::Event_t;

        std::string ToString() const override
        {
            return "WindowSizeChangedEvent";
        }
    };
}
