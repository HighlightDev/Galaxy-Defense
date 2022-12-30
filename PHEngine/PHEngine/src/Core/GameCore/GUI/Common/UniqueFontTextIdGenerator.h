#pragma once

#include <cstdint>
#include <stdint.h>
#include <atomic>

namespace EngineCore
{
    struct UniqueFontTextIdGenerator
    {
        static std::atomic<int32_t> s_TotalTextFieldId;

        static int32_t GenerateUniqueFontTextId();
    };
}