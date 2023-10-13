#pragma once

namespace EngineCore
{
    class ACamera;
}

namespace Game
{
    struct ICameraTransformChangeNotifyable
    {
        virtual void OnCameraTransformChanged(::EngineCore::ACamera* eventSrc) = 0;
    };
}