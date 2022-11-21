#pragma once

#include <string>
#include <cstdint>
#include <stdint.h>

namespace EngineCore
{
    namespace Scripts
    {
        class ILuaScriptExecutor
        {
            virtual void SetScript(const std::string &scriptName) = 0;

            virtual std::string GetScriptName() const = 0;
            virtual size_t GetUId() const = 0;
        };
    }
}