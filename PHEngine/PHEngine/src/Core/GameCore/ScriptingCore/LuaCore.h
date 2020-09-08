#pragma once
#include <string>
#include <stdint.h>

extern "C"
{
#include <lua/lua.h>
#include <lua/lualib.h>
#include <lua/lauxlib.h>
}

#include "Core/CommonCore/Assertion.h"
#include "LuaWrapper.h"

namespace Game
{
   /*------------ Inner Core  --------------*/
   
   template <typename ArgType>
   struct IsVoid
   {
      enum {
         value = false
      };
   };

   template <>
   struct IsVoid<void>
   {
      enum {
         value = true
      };
   };


   template <typename ArgType>
   struct PushValue;

   template <>
   struct PushValue<double>
   {
      static void Do(const LuaWrapper& instanceWrapper, const double& value)
      {
         lua_pushnumber(instanceWrapper.GetState(), value);
      }
   };

   template <>
   struct PushValue<int64_t>
   {
      static void Do(const LuaWrapper& instanceWrapper, const int64_t& value)
      {
         lua_pushinteger(instanceWrapper.GetState(), value);
      }
   };

   template <typename... Args>
   struct IterateFunctionArgs;

   template <typename Arg, typename... Args>
   struct IterateFunctionArgs<Arg, Args...>
   {
      static void PushArg(const LuaWrapper& instanceWrapper, size_t& argsCount, Args&&... args, Arg&& arg)
      {
         ++argsCount;
         PushValue<Arg>::Do(instanceWrapper, std::forward<Arg>(arg));
         IterateFunctionArgs<Args...>::PushArg(instanceWrapper, argsCount, std::forward<Args>(args)...);
      }
   };

   template <>
   struct IterateFunctionArgs<>
   {
      static void PushArg(const LuaWrapper& instanceWrapper, size_t& argsCount)
      {
      }
   };


   template <typename VariableType>
   struct GetValue;

   template <>
   struct GetValue<int64_t>
   {
      static int64_t Value(const LuaWrapper& instanceWrapper, const int32_t stackIndex)
      {
         assert(lua_isinteger(instanceWrapper.GetState(), stackIndex));
         return lua_tointeger(instanceWrapper.GetState(), stackIndex);
      }
   };

   template <>
   struct GetValue<double>
   {
      static double Value(const LuaWrapper& instanceWrapper, const int32_t stackIndex)
      {
         assert(lua_isnumber(instanceWrapper.GetState(), stackIndex));
         return lua_tonumber(instanceWrapper.GetState(), stackIndex);
      }
   };

   template <>
   struct GetValue<void>
   {
      static double Value(const LuaWrapper& instanceWrapper, const int32_t stackIndex)
      {
      }
   };

   /*------------ Inner Core  --------------*/

   template <typename GlobalVariableType>
   struct LuaGetGlobal;

   template <>
   struct LuaGetGlobal<int64_t>
   {
      static int64_t Value(const LuaWrapper& instanceWrapper, const std::string& variableName, const int32_t stackIndex)
      {
         lua_getglobal(instanceWrapper.GetState(), variableName.c_str());
         GetValue<int64_t>::Value(instanceWrapper, stackIndex);
      }
   };

   template <>
   struct LuaGetGlobal<double>
   {
      static double Value(const LuaWrapper& instanceWrapper, const std::string& variableName, const int32_t stackIndex)
      {
         lua_getglobal(instanceWrapper.GetState(), variableName.c_str());
         GetValue<double>::Value(instanceWrapper, stackIndex);
      }
   };

   template <typename FunctionType>
   struct LuaFunction;

   template <typename RetType, typename... Args>
   struct LuaFunction<RetType(Args...)>
   {

      static RetType Call(const LuaWrapper& instanceWrapper, const std::string& functionName, Args... args)
      {
         lua_getglobal(instanceWrapper.GetState(), functionName.c_str());
         assert(lua_isfunction(instanceWrapper.GetState(), -1));
        
         static constexpr bool bRetTypeIsVoid = IsVoid<RetType>::value;
         size_t argsCount = 0;
         IterateFunctionArgs<Args...>::PushArg(instanceWrapper, argsCount, std::forward<Args>(args)...);

         if (LUA_OK != lua_pcall(instanceWrapper.GetState(), argsCount, bRetTypeIsVoid ? 0 : 1, /*error handling in lua*/0))
         {
            std::cout << instanceWrapper.GetErrorMessageAt(-1) << std::endl;
         }

         if (!bRetTypeIsVoid)
         {
            return GetValue<RetType>::Value(instanceWrapper, -1);
         }
      }
   };
}