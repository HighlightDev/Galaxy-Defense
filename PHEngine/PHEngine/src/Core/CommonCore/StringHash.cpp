
#include "StringHash.h"

namespace EngineCore
{
   uint64_t Hash(const char* str, const size_t hVal)
   {
      using FNV_32_PRIME = std::integral_constant<uint32_t, (1 << 24) + (1 << 8) + 0x93>;
      return 0 != *str ? Hash(str + 1, static_cast<uint64_t>(static_cast<uint32_t>(hVal ^ *str) * static_cast<uint64_t>(FNV_32_PRIME::value))) : hVal;
   }

   uint64_t Hash(const std::string& str)
   {
      return Hash(str.c_str());
   }
}
