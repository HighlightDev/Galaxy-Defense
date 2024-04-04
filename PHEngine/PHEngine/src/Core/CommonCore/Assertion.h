#pragma once
#include <cassert>

#ifdef DEBUG
#undef NDEBUG
#include <iostream>
#define ext_assert(condition, message)      \
   do                                       \
   {                                        \
      if (!condition)                       \
      {                                     \
         std::cout << message << std::endl; \
      }                                     \
      assert(condition);                    \
   } while (0)
#else
#define NDEBUG
#define ext_assert(condition, message) void(0)
#endif
