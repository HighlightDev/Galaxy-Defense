#pragma once

#include "Core.h"

#include <stdio.h>

#include <chrono>
#include <ctime>
#include <istream>
#include <numeric>
#include <string>
#include <thread>
#include <type_traits>
#include <utility>

namespace TinyLogger {

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define AT "File: " __FILE__ "; Line: " TOSTRING(__LINE__) "; Function: " TOSTRING(__FUNCTION__) ";"

namespace LogHelp {
template<typename T>
static typename std::enable_if<!std::is_same<typename std::decay<T>::type, const char*>::value, T>::type CompressMessage(T&& arg)
{
    return std::forward<T>(arg);
}

template<typename T>
static typename std::enable_if<std::is_same<typename std::decay<T>::type, const char*>::value, std::string>::type
CompressMessage(const char* arg)
{
    return std::string(arg);
}

struct ToString {

    template<typename U>
    static std::string Value(const U& value)
    {
        if constexpr ((std::is_integral_v<U>) || (std::is_floating_point_v<U>)) {
            return std::to_string(value);
        } else if constexpr (std::is_same_v<U, std::string>) {
            return value;
        } else if constexpr (std::is_same_v<U, const char*>) {
            return std::string(value);
        }
        return "Unknown argument type";
    }
};

template<typename T>
struct CastTypeToString {
    static std::string Do(T notStr)
    {
        return ToString::Value<T>(notStr);
    }
};

template<>
struct CastTypeToString<std::string> {
    static std::string Do(const std::string& str)
    {
        return str;
    }
};

template<>
struct CastTypeToString<std::tuple<>> {
    static std::string Do(const std::tuple<>& tuple)
    {
        return "|EmptyTuple|";
    }
};

template<typename TupleT, size_t max_index, size_t index>
struct IterateTuple {
    static void Collect(std::vector<std::string>& result, TupleT& tuple)
    {
        using tuple_arg_t = typename std::tuple_element<index, TupleT>::type;
        auto argument = CompressMessage<tuple_arg_t>(std::forward<tuple_arg_t>(std::get<index>(tuple)));
        using compressed_arg_t = typename std::decay<decltype(argument)>::type;

        result.push_back(CastTypeToString<compressed_arg_t>::Do(std::forward<compressed_arg_t>(argument)));
        IterateTuple<TupleT, max_index, index + 1>::Collect(result, tuple);
    }
};

template<typename... TupleArgs>
struct CastTypeToString<std::tuple<TupleArgs...>> {
    static std::string Do(std::tuple<TupleArgs...> tuple)
    {
        using tuple_t = std::tuple<TupleArgs...>;
        std::vector<std::string> innerTupleArgumentsStr;
        constexpr size_t tupleSize = std::tuple_size<tuple_t>();
        innerTupleArgumentsStr.reserve(tupleSize);
        IterateTuple<tuple_t, tupleSize, 0>::Collect(innerTupleArgumentsStr, tuple);
        return std::accumulate(
            innerTupleArgumentsStr.cbegin(),
            innerTupleArgumentsStr.cend(),
            std::string(),
            [](const std::string& accumulatedStr, const std::string& argument) { return accumulatedStr + ", " + argument; });
    }
};

template<typename TupleT, size_t max_index>
struct IterateTuple<TupleT, max_index, max_index> {
    static void Collect(std::vector<std::string>& result, TupleT& tuple)
    {
    }
};
} // namespace LogHelp

#ifdef _WIN32
#ifdef TINYLOGGER_EXPORTS
#define TINYLOGGER_API __declspec(dllexport)
#else if defined(TINYLOGGER_IMPORTS)
#define TINYLOGGER_API __declspec(dllimport)
#endif
#else
#define TINYLOGGER_API
#endif

struct Logger {
    using Clock_t = std::chrono::high_resolution_clock;
    using Duration_t = Clock_t::duration;
    using Moment_t = std::chrono::time_point<std::chrono::system_clock>;

    static size_t index;
    static Moment_t logStartTimestamp;

    template<typename... LogArgs>
    static void Out(const std::string& threadName, LogArgs&&... args)
    {
        static std::hash<std::thread::id> hasher;
        const auto timestampNow = std::chrono::system_clock::now();

        static constexpr double invFromNanoToSec = 1e-9;
        const double timePassedSinceStart
            = std::chrono::duration_cast<std::chrono::nanoseconds>(timestampNow - logStartTimestamp).count() * invFromNanoToSec;

        std::vector<std::string> result{
            std::to_string(index++), "| Timestamp: " + std::to_string(timePassedSinceStart), "| Thread: " + threadName + "| "};

        auto argTuple = std::make_tuple(std::forward<LogArgs>(args)...);
        using tuple_t = std::decay_t<decltype(argTuple)>;
        constexpr size_t size = std::tuple_size<tuple_t>();
        LogHelp::IterateTuple<tuple_t, size, 0>::Collect(result, argTuple);
        LoggerServer::GetInstance_()->EnqueuLogMessage(LogMessage(std::move(result)));
    }

    /* initialization should be called before any action with log*/

    static void InitLog()
    {
    }

    template<typename LoggerClient, typename... Args>
    static void InitLog(LoggerClient&& loggerClient, Args&&... clients)
    {
        LoggerServer::GetInstance_()->AddLoggerClient(loggerClient);
        InitLog(std::move(clients)...);
    }

    static void StartLogThread()
    {
        LoggerServer::GetInstance_()->StartLogThread();
        logStartTimestamp = std::chrono::system_clock::now();
    }

    static void StopLogThread()
    {
        LoggerServer::GetInstance_()->StopLogThread();
    }
};
} // namespace TinyLogger