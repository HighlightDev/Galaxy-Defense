#pragma once 

#include <thread>
#include <istream>
#include <utility>
#include <stdio.h>
#include <thread>
#include <ctime>
#include <chrono> 
#include <string>

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
         enum {
            value = false
         };
      };

      struct TrueType
      {
         enum {
            value = true
         };
      };

      template <typename T>
      struct IsDefaultType : public FalseType {};

      template <> struct IsDefaultType<int64_t> : public TrueType {};
      template <> struct IsDefaultType<int32_t> : public TrueType {};
      template <> struct IsDefaultType<int8_t> : public TrueType {};
      template <> struct IsDefaultType<uint64_t> : public TrueType {};
      template <> struct IsDefaultType<uint32_t> : public TrueType {};
      template <> struct IsDefaultType<uint8_t> : public TrueType {};
      template <> struct IsDefaultType<float> : public TrueType {};
      template <> struct IsDefaultType<double> : public TrueType {};
      template <> struct IsDefaultType<bool> : public TrueType {};

      template <typename T>
      struct ToString
      {
         template <typename U>
         static typename std::enable_if<IsDefaultType<T>::value, std::string>::type Value(U simpleType)
         {
            return std::to_string(simpleType);
         }
      };

      template <typename T> struct IsString { enum { value = false }; };
      template <> struct IsString<std::string> { enum { value = false }; };

      template <typename T, bool>
      struct DoIfTrue
      {
         static std::string Do(T notStr)
         {
            return ToString<T>::Value(notStr);
         }
      };

      template <>
      struct DoIfTrue<std::string, false>
      {
         static std::string Do(const std::string& str)
         {
            return str;
         }
      };

      template <typename TupleT, size_t index>
      struct IterateTuple
      {
         static void Collect(std::vector<std::string>& result, TupleT& tuple)
         {
            auto value = std::get<index>(tuple);

            using arg_t = typename std::tuple_element<index, TupleT>::type;
            result.push_back(DoIfTrue<IsString<arg_t>::value>::Do(value));
            IterateTuple<TupleT, index - 1>::Collect(result, tuple);
         }
      };
      
      template <typename TupleT>
      struct IterateTuple<TupleT, -1>
      {
         static void Collect(std::vector<std::string>& result, TupleT& tuple)
         {
         }
      };

      template <typename TupleT>
      struct IterateToString
      {

         static void Do(std::vector<std::string>& collectedStrings, TupleT& tup)
         {
            constexpr size_t size = sizeof(TupleT);

            IterateTuple<TupleT, size - 1>::Collect(collectedStrings, tup);
         }
      };
   }

   struct LogProxy
   {
      template <typename LogArg, typename... LogArgs>
      static void LogMessages(LogArg arg, LogArgs... args)
      {
         static std::hash<std::thread::id> hasher;

         std::chrono::system_clock::time_point p = std::chrono::system_clock::now();
         typename std::chrono::system_clock::time_point time = std::chrono::system_clock::now();
         std::time_t currentTime = std::chrono::system_clock::to_time_t(time);

         std::string timeFileWasChanged = std::asctime(std::localtime(&currentTime));
         timeFileWasChanged[timeFileWasChanged.size() - 1] = ' ';

         //std::string currentTimeStr = std::to_string() + " ";

         std::vector<std::string> result;
         using tuple_t = std::tuple<LogArgs...>;
         tuple_t tup = std::make_tuple<LogArgs...>(std::forward<LogArgs>(args)...);
         LogHelp::IterateToString<tuple_t>::Do(result, tup);

         //std::initializer_list<std::string> initList({ timeFileWasChanged, "Thread: " + std::to_string(hasher(std::this_thread::get_id())), arg, args... });

         //Logger::GetInstance_()->EnqueuLogMessage(LogMessage(initList));
      }

#define LOG_INFO (AT)

      /* initialization should be called before any action with log*/

      static void InitLog() {

      }

      template <typename LoggerClient, typename... Args>
      static void InitLog(LoggerClient* loggerClient, Args&&... clients)
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