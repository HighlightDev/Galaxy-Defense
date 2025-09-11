#pragma once
#include <cassert>

#ifdef DEBUG
#undef NDEBUG
#include "Core/GameCore/LoggerExtension.h"

#include <TinyLogger/LogInterface.h>

#define ext_assert(condition, message)                                                                                           \
    do {                                                                                                                         \
        if (!condition) {                                                                                                        \
            EngineCore::LogInfo("Assertion failed! ", message);                                                                  \
            TinyLogger::Logger::StopLogThread();                                                                                 \
        }                                                                                                                        \
        assert(condition);                                                                                                       \
    } while (0)
#else
#define NDEBUG
#define ext_assert(condition, message) void(0)
#endif
