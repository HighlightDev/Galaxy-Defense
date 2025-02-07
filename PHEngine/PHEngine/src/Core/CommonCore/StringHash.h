#pragma once

#include <climits>
#include <cstddef>
#include <cstdint>
#include <string>
#include <type_traits>

namespace EngineCore {
// Implementation of FNV (Fowler-Noll-Vo) hashing algorithm, version FNV-1a
uint64_t Hash(const char* str, const size_t hVal = 0x811c9dc5);

uint64_t Hash(const std::string& str);

constexpr uint64_t Hash64_CT(const char* str, size_t n, uint64_t basis = UINT64_C(14695981039346656037))
{
    return n == 0 ? basis : Hash64_CT(str + 1, n - 1, (basis ^ str[0]) * UINT64_C(1099511628211));
}

template<size_t N>
constexpr uint64_t Hash64_CT(const char (&s)[N])
{
    return Hash64_CT(s, N - 1);
}
} // namespace EngineCore
