#pragma once

#include <thread>
#include <istream>
#include <utility>
#include <stdio.h>
#include <thread>
#include <ctime>
#include <chrono>
#include <string>
#include <type_traits>

#include "Core.h"

namespace TinyLogger
{

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define AT "File: " __FILE__ "; Line: " TOSTRING(__LINE__) "; Function: " TOSTRING(__FUNCTION__) ";"

   namespace LogHelp
   {
      template <typename T>
      static typename std::enable_if<!std::is_same<typename std::decay<T>::type, const char *>::value, T>::type CompressMessage(T &&arg)
      {
         return std::forward<T>(arg);
      }

      template <typename T>
      static typename std::enable_if<std::is_same<typename std::decay<T>::type, const char *>::value, std::string>::type CompressMessage(const char *arg)
      {
         return std::string(arg);
      }

      struct FalseType
      {
         enum
         {
            value = false
         };
      };

      struct TrueType
      {
         enum
         {
            value = true
         };
      };

      template <typename T>
      struct IsDefaultType : public FalseType
      {
      };

      template <>
      struct IsDefaultType<int64_t> : public TrueType
      {
      };
      template <>
      struct IsDefaultType<int32_t> : public TrueType
      {
      };
      template <>
      struct IsDefaultType<int8_t> : public TrueType
      {
      };
      template <>
      struct IsDefaultType<uint64_t> : public TrueType
      {
      };
      template <>
      struct IsDefaultType<uint32_t> : public TrueType
      {
      };
      template <>
      struct IsDefaultType<uint8_t> : public TrueType
      {
      };
      template <>
      struct IsDefaultType<float> : public TrueType
      {
      };
      template <>
      struct IsDefaultType<double> : public TrueType
      {
      };
      template <>
      struct IsDefaultType<bool> : public TrueType
      {
      };

      template <typename T>
      struct ToString
      {
         template <typename U>
         static typename std::enable_if<IsDefaultType<typename std::decay<T>::type>::value, std::string>::type Value(U simpleType)
         {
            return std::to_string(simpleType);
         }
      };

      template <typename T>
      struct CastTypeToString
      {
         static std::string Do(T notStr)
         {
            return ToString<T>::Value(notStr);
         }
      };

      template <>
      struct CastTypeToString<std::string>
      {
         static std::string Do(const std::string &str)
         {
            return str;
         }
      };

      template <typename TupleT, size_t max_index, size_t index>
      struct IterateTuple
      {
         static void Collect(std::vector<std::string> &result, TupleT &tuple)
         {
            using tuple_arg_t = typename std::tuple_element<index, TupleT>::type;
            auto argument = CompressMessage<tuple_arg_t>(std::forward<tuple_arg_t>(std::get<index>(tuple)));
            using compressed_arg_t = decltype(argument);

            result.push_back(CastTypeToString<compressed_arg_t>::Do(std::forward<compressed_arg_t>(argument)));
            IterateTuple<TupleT, max_index, index + 1>::Collect(result, tuple);
         }
      };

      template <typename TupleT, size_t max_index>
      struct IterateTuple<TupleT, max_index, max_index>
      {
         static void Collect(std::vector<std::string> &result, TupleT &tuple)
         {
         }
      };
   }

   template <typename T>
   struct GetCompressedMessageType
   {
      using type = typename std::decay<T>::type;
   };

   template <>
   struct GetCompressedMessageType<const char *>
   {
      using type = std::string;
   };

   struct Logger
   {
      using Clock_t = std::chrono::high_resolution_clock;
      using Duration_t = Clock_t::duration;
      using Moment_t = Clock_t::time_point;

      static size_t index;
      static Clock_t::time_point logStartTimestamp;

      template <typename LogArg, typename... LogArgs>
      static void Out(LogArg &&arg, LogArgs &&...args)
      {
         static std::hash<std::thread::id> hasher;
         const auto timestampNow = std::chrono::system_clock::now();

         static constexpr double invFromNanoToSec = 1e-9;
         const double timePassedSinceStart = static_cast<double>((timestampNow - logStartTimestamp).count()) * invFromNanoToSec;

         auto argument = LogHelp::CompressMessage<LogArg>(std::forward<LogArg>(arg));
         using argument_t = typename GetCompressedMessageType<typename std::decay<LogArg>::type>::type;
         auto argTuple = std::make_tuple(std::forward<argument_t>(argument),
                                         std::forward<LogArgs>(args)...);

         using tuple_t = decltype(argTuple);

         std::vector<std::string> result{std::to_string(index),
                                         "| Timestamp: " + std::to_string(timePassedSinceStart),
                                         "| Thread: " + std::to_string(hasher(std::this_thread::get_id())) + "| "};
         ++index;
         constexpr size_t size = std::tuple_size<tuple_t>();
         LogHelp::IterateTuple<tuple_t, size, 0>::Collect(result, argTuple);

         LoggerServer::GetInstance_()->EnqueuLogMessage(LogMessage(std::move(result)));
      }

#define LOG_INFO (AT)

      /* initialization should be called before any action with log*/

      static void InitLog()
      {
      }

      template <typename LoggerClient, typename... Args>
      static void InitLog(LoggerClient *loggerClient, Args &&...clients)
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
}