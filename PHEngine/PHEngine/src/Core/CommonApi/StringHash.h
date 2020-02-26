#pragma once

#include <string>
#include <type_traits>
#include <stdint.h>

namespace Game
{
   // Implementation of FNV (Fowler-Noll-Vo) hashing algorithm, version FNV-1a
   constexpr uint64_t Hash(const char* str, const size_t hVal = 0x811c9dc5)
   {
     using FNV_32_PRIME = std::integral_constant<uint32_t, (1 << 24) + (1 << 8) + 0x93>;
   return 0 != *str ? Hash(str + 1, static_cast<uint64_t>(static_cast<uint32_t>(hVal ^ *str) * static_cast<uint64_t>(FNV_32_PRIME::value))) : hVal;
   }

   constexpr uint64_t Hash(const std::string& str)
   {
      return Hash(str.c_str());
   }
}
