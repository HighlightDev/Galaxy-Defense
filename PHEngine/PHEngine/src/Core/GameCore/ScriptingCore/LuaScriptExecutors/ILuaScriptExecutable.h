#pragma once

#include <string>
#include <cstdint>
#include <stdint.h>

namespace EngineCore
{
    namespace Scripts
    {
        class ILuaScriptExecutable
        {
        public:
            virtual void RegisterCallbacks() = 0;

            virtual void RunScript() = 0;

            virtual void StopScript() = 0;

            virtual void OnUpdate(const float deltaTime) = 0;

            virtual void CleanUp() = 0;
        };
    }
}