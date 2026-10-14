#pragma once
#include <cassert>

#ifdef DEBUG
#undef NDEBUG
#include "Core/GameCore/LoggerExtension.h"

#include <TinyLogger/LogInterface.h>

#ifdef USE_LIBUNWIND
#include "TinyUnwinder.h"
#define ext_assert(condition, message)                                                                                           \
    do {                                                                                                                         \
        if (!(condition)) {                                                                                                      \
            Tools::Unwind::TinyUnwinder unwinder;                                                                                \
            EngineCore::LogInfo("Assertion failed! ", message, "\nCallstack BackTrace: \n", unwinder.GetStackBacktraceStr());    \
            TinyLogger::Logger::StopLogThread();                                                                                 \
        }                                                                                                                        \
        assert(condition);                                                                                                       \
    } while (0)
#else
#define ext_assert(condition, message)                                                                                           \
    do {                                                                                                                         \
        if (!(condition)) {                                                                                                      \
            EngineCore::LogInfo("Assertion failed! ", message);                                                                  \
            TinyLogger::Logger::StopLogThread();                                                                                 \
        }                                                                                                                        \
        assert(condition);                                                                                                       \
    } while (0)
#endif // USE_LIBUNWIND

#else
#define NDEBUG
#define ext_assert(condition, message) void(0)
#endif // DEBUG
