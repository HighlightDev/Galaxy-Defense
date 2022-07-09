#pragma once

namespace EngineCore
{
    struct ITweenStateChangeNotifyable
    {
        virtual void OnTweenStateChanged(const std::string& stateName) = 0;
    };
}