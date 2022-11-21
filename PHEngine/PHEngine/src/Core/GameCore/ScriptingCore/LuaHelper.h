#pragma once

#include "LuaWrapper.h"

namespace EngineCore
{
    namespace Scripts
    {
        bool HasLuaError(const LuaWrapper &instanceWrapper, int32_t luaCallResult);
    }
}