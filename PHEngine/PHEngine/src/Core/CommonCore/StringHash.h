#pragma once

#include <string>
#include <type_traits>
#include <cstddef>

namespace Game
{
   // Implementation of FNV (Fowler-Noll-Vo) hashing algorithm, version FNV-1a
   uint64_t Hash(const char* str, const size_t hVal = 0x811c9dc5);

   uint64_t Hash(const std::string& str);
}
