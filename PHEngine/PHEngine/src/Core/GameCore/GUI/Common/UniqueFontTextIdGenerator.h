#pragma once

#include <stdint.h>

#include <atomic>
#include <cstdint>

namespace EngineCore {
struct UniqueFontTextIdGenerator {
    static std::atomic<int32_t> s_TotalTextFieldId;

    static int32_t GenerateUniqueFontTextId();
};
} // namespace EngineCore