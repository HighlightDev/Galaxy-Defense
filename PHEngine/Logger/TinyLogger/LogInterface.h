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
            using arg_t = typename std::tuple_element<index, TupleT>::type;

            result.push_back(CastTypeToString<arg_t>::Do(std::forward<arg_t>(std::get<index>(tuple))));
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

   struct LogProxy
   {
      static size_t index;
      template <typename LogArg, typename... LogArgs>
      static void LogMessages(LogArg &&arg, LogArgs &&...args)
      {
         static std::hash<std::thread::id> hasher;
         const std::time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

#ifdef _WIN32
         char str[26];
         struct tm timeinfo;
         localtime_s(&timeinfo, &currentTime);
         asctime_s(str, sizeof str, &timeinfo);
#elif __linux__
         const std::string& str =std::asctime(std::localtime(&currentTime));
#endif
         std::string timeFileWasChanged = str;
         timeFileWasChanged[timeFileWasChanged.size() - 1] = ' ';

         auto argument = CompressMessage<LogArg>(std::forward<LogArg>(arg));
         using argument_t = typename GetCompressedMessageType<typename std::decay<LogArg>::type>::type;
         using tuple_t = std::tuple<argument_t, LogArgs...>;
         tuple_t argTuple = std::make_tuple<argument_t, LogArgs...>(std::forward<argument_t>(argument),
                                                                    std::forward<LogArgs>(args)...);

         std::vector<std::string> result{std::to_string(index), timeFileWasChanged, "Thread: " + std::to_string(hasher(std::this_thread::get_id()))};
         ++index;
         constexpr size_t size = std::tuple_size<tuple_t>();
         LogHelp::IterateTuple<tuple_t, size, 0>::Collect(result, argTuple);

         Logger::GetInstance_()->EnqueuLogMessage(LogMessage(result));
      }

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

#define LOG_INFO (AT)

      /* initialization should be called before any action with log*/

      static void InitLog()
      {
      }

      template <typename LoggerClient, typename... Args>
      static void InitLog(LoggerClient *loggerClient, Args &&...clients)
      {
         Logger::GetInstance_()->AddLoggerClient(loggerClient);
         InitLog(std::move(clients)...);
      }

      static void StartLogThread()
      {
         Logger::GetInstance_()->StartLogThread();
      }
   };
}