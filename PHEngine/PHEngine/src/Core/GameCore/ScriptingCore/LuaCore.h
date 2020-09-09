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
   
   struct LightUserData
   {
      void* data;

      LightUserData(void* d)
         : data(d)
      {
      }
   };

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
   struct PushValue<LightUserData>
   {
      static void Do(const LuaWrapper& instanceWrapper, LightUserData value)
      {
         lua_pushlightuserdata(instanceWrapper.GetState(), value.data);
      }
   };

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
      static void PushArg(const LuaWrapper& instanceWrapper, Arg&& arg, Args&&... args)
      {
         PushValue<Arg>::Do(instanceWrapper, std::forward<Arg>(arg));
         IterateFunctionArgs<Args...>::PushArg(instanceWrapper, std::forward<Args>(args)...);
      }
   };

   template <>
   struct IterateFunctionArgs<>
   {
      static void PushArg(const LuaWrapper& instanceWrapper)
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

      template <typename... TArgs>
      static RetType Call(const LuaWrapper& instanceWrapper, const std::string& functionName, TArgs&&... args)
      {
         lua_getglobal(instanceWrapper.GetState(), functionName.c_str());
         assert(lua_isfunction(instanceWrapper.GetState(), -1));
        
         IterateFunctionArgs<TArgs...>::PushArg(instanceWrapper, std::forward<TArgs>(args)...);

         static constexpr size_t argsCount = sizeof...(args);
         if (LUA_OK != lua_pcall(instanceWrapper.GetState(), argsCount, 1, /*error handling in lua*/0))
         {
            std::cout << instanceWrapper.GetErrorMessageAt(-1) << std::endl;
         }

         return GetValue<RetType>::Value(instanceWrapper, -1);
      }
   };

   template <typename... Args>
   struct LuaFunction<void(Args...)>
   {

      template <typename... TArgs>
      static void Call(const LuaWrapper& instanceWrapper, const std::string& functionName, TArgs&&... args)
      {
         lua_getglobal(instanceWrapper.GetState(), functionName.c_str());
         assert(lua_isfunction(instanceWrapper.GetState(), -1));

         IterateFunctionArgs<TArgs...>::PushArg(instanceWrapper, std::forward<TArgs>(args)...);

         static constexpr size_t argsCount = sizeof...(args);
         if (LUA_OK != lua_pcall(instanceWrapper.GetState(), argsCount, 0, /*error handling in lua*/0))
         {
            std::cout << instanceWrapper.GetErrorMessageAt(-1) << std::endl;
         }
      }
   };

   template <typename ILuaExecutor, typename FunctionType>
   struct LuaRegisterCallback;


   template <typename ILuaExecutor, typename RetType, typename... Args>
   struct LuaRegisterCallback<ILuaExecutor, RetType(Args...)>
   {
      using type = LuaRegisterCallback<ILuaExecutor, RetType(Args...)>;

      static int Wrapper(lua_State* state)
      {
         if (lua_gettop(state) != 2) return -1;

         ILuaExecutor* instance = static_cast<ILuaExecutor*>(lua_touserdata(state, 1));
         
         double value = lua_tonumber(state, 2);
         instance->operator()(value);
         return 0;
      }

      static RetType Rigister(const LuaWrapper& instanceWrapper, const std::string& functionName)
      {
         lua_register(instanceWrapper.GetState(), functionName.c_str(), type::Wrapper);
      }
   };
}