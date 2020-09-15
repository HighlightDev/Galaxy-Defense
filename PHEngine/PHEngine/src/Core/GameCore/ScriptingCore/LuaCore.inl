#pragma once
#include <string>
#include <stdint.h>
#include <tuple>
#include <type_traits>

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
   struct LuaTableBase
   {
   };

   template <typename... Args>
   struct LuaTable :
      public LuaTableBase,
      public std::tuple<Args...>
   {
   };

   namespace LuaInnerCore
   {
      /*------------ Inner Core  --------------*/
      struct CheckLuaExecution
      {
         static bool Do(const LuaWrapper& instanceWrapper, int32_t luaCallResult)
         {
            if (LUA_OK != luaCallResult)
            {
               std::cout << instanceWrapper.GetErrorMessageAt(-1) << std::endl;
               return false;
            }

            return true;
         }
      };

      template <typename ArgType>
      struct PushValue;

      template <typename ArgType, bool isPtr>
      struct PushUnknownValue;

      template <typename ArgType>
      struct PushUnknownValue<ArgType, true>
      {
         static void Do(lua_State* state, const ArgType& value)
         {
            PushValue<void*>::Do(state, (void*)value);
         }
      };

      template <typename ArgType>
      struct PushUnknownValue<ArgType, false>
      {
         static void Do(lua_State* state, const ArgType& value)
         {
            PushValue<ArgType>::Do(state, value);
         }
      };

      template <>
      struct PushValue<void*>
      {
         static void Do(lua_State* state, void* value)
         {
            lua_pushlightuserdata(state, value);
         }
      };

      template <>
      struct PushValue<std::string>
      {
         static void Do(lua_State* state, const std::string& value)
         {
            lua_pushstring(state, value.c_str());
         }
      };

      template <>
      struct PushValue<float>
      {
         static void Do(lua_State* state, const float& value)
         {
            lua_pushnumber(state, value);
         }
      };

      template <>
      struct PushValue<int32_t>
      {
         static void Do(lua_State* state, const int32_t& value)
         {
            lua_pushinteger(state, value);
         }
      };

      template <>
      struct PushValue<double>
      {
         static void Do(lua_State* state, const double& value)
         {
            lua_pushnumber(state, value);
         }
      };

      template <>
      struct PushValue<int64_t>
      {
         static void Do(lua_State* state, const int64_t& value)
         {
            lua_pushinteger(state, value);
         }
      };

      template <typename... Args>
      struct IterateFunctionArgs;

      template <typename Arg, typename... Args>
      struct IterateFunctionArgs<Arg, Args...>
      {
         static void PushArg(const LuaWrapper& instanceWrapper, Arg&& arg, Args&&... args)
         {
            PushValue<Arg>::Do(instanceWrapper.GetState(), std::forward<Arg>(arg));
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
      struct GetValue
      {
         static VariableType Value(const LuaWrapper& instanceWrapper, const int32_t stackIndex)
         {
            return Inner_Value(instanceWrapper.GetState(), stackIndex);
         }

         static VariableType Value(lua_State* state, const int32_t stackIndex)
         {
            return Inner_Value(state, stackIndex);
         }

      private:

         static typename std::enable_if<std::is_pointer<VariableType>::value, VariableType>::type Inner_Value(lua_State* state, const int32_t stackIndex)
         {
            return (VariableType)lua_touserdata(state, stackIndex);
         }
      };

      template <>
      struct GetValue<std::string>
      {
      public:

         static std::string Value(const LuaWrapper& instanceWrapper, const int32_t stackIndex)
         {
            return Inner_Value(instanceWrapper.GetState(), stackIndex);
         }

         static std::string Value(lua_State* state, const int32_t stackIndex)
         {
            return Inner_Value(state, stackIndex);
         }

      private:

         static std::string Inner_Value(lua_State* state, const int32_t stackIndex)
         {
            assert(lua_isstring(state, stackIndex));
            return lua_tostring(state, stackIndex);
         }
      };

      template <>
      struct GetValue<int64_t>
      {
      public:

         static int64_t Value(const LuaWrapper& instanceWrapper, const int32_t stackIndex)
         {
            return Inner_Value(instanceWrapper.GetState(), stackIndex);
         }

         static int64_t Value(lua_State* state, const int32_t stackIndex)
         {
            return Inner_Value(state, stackIndex);
         }

      private:

         static int64_t Inner_Value(lua_State* state, const int32_t stackIndex)
         {
            assert(lua_isinteger(state, stackIndex));
            return lua_tointeger(state, stackIndex);
         }
      };

      template <>
      struct GetValue<double>
      {
      public:

         static double Value(const LuaWrapper& instanceWrapper, const int32_t stackIndex)
         {
            return Inner_Value(instanceWrapper.GetState(), stackIndex);
         }

         static double Value(lua_State* state, const int32_t stackIndex)
         {
            return Inner_Value(state, stackIndex);
         }

      private:

         static double Inner_Value(lua_State* state, const int32_t stackIndex)
         {
            assert(lua_isnumber(state, stackIndex));
            return lua_tonumber(state, stackIndex);
         }
      };

      template <>
      struct GetValue<int32_t>
      {
      public:

         static int32_t Value(const LuaWrapper& instanceWrapper, const int32_t stackIndex)
         {
            return Inner_Value(instanceWrapper.GetState(), stackIndex);
         }

         static int32_t Value(lua_State* state, const int32_t stackIndex)
         {
            return Inner_Value(state, stackIndex);
         }

      private:

         static int32_t Inner_Value(lua_State* state, const int32_t stackIndex)
         {
            assert(lua_isinteger(state, stackIndex));
            return (int32_t)lua_tointeger(state, stackIndex);
         }
      };

      template <>
      struct GetValue<float>
      {
      public:

         static float Value(const LuaWrapper& instanceWrapper, const int32_t stackIndex)
         {
            return Inner_Value(instanceWrapper.GetState(), stackIndex);
         }

         static float Value(lua_State* state, const int32_t stackIndex)
         {
            return Inner_Value(state, stackIndex);
         }

      private:

         static float Inner_Value(lua_State* state, const int32_t stackIndex)
         {
            assert(lua_isnumber(state, stackIndex));
            return (float)lua_tonumber(state, stackIndex);
         }
      };

      template <>
      struct GetValue<void>
      {
         static void Value(const LuaWrapper& instanceWrapper, const int32_t stackIndex)
         {
         }
      };

      template <size_t LuaTableParamCount, typename tuple_type, typename LuaTableType>
      struct GetLuaTableValue;

      template <size_t LuaTableParamCount, typename tuple_type, typename... Args>
      struct GetLuaTableValue<LuaTableParamCount, tuple_type, LuaTable<Args...>>
      {
         static void Value(const LuaWrapper& instanceWrapper, const int32_t stackIndex)
         {
         }
      };

      template <typename tuple_type, typename... Args>
      struct GetLuaTableValue<0, tuple_type, LuaTable<Args...>>
      {

      };

      template <typename T>
      struct IsLuaTable
      {
         enum
         {
            value = std::is_base_of<LuaTableBase, T>::value
         };
      };

      template <typename tuple_type, size_t argsCount>
      struct CollectArgsFromLuaHostInvoke;

      template <typename tuple_type, size_t argsCount>
      struct CollectArgsFromLuaHostInvoke
      {
         using arg_type = typename std::tuple_element<argsCount - 1, tuple_type>::type;

         static void Collect(lua_State* state, tuple_type& params)
         {
            std::get<argsCount - 1>(params) = GetValue<arg_type>::Value(state, argsCount + 1); // + 1 because of host data at index 1
            CollectArgsFromLuaHostInvoke<tuple_type, argsCount - 1>::Collect(state, params);
         }
      };

      template <typename tuple_type>
      struct CollectArgsFromLuaHostInvoke<tuple_type, 0>
      {
         static void Collect(lua_State* state, tuple_type& params)
         {
         }
      };

      struct LuaGetGlobalBase
      {
         static void GetGlobal(const LuaWrapper& instanceWrapper, const std::string& variableName)
         {
            lua_getglobal(instanceWrapper.GetState(), variableName.c_str());
         }
      };

      template <typename ILuaExecutor_t, typename ArgsPack_t, typename ReturnValueType>
      struct LuaCallbackReturnValue
      {
         static int PushToLua(lua_State* state, ILuaExecutor_t* executorInstance, ArgsPack_t& packArgs)
         {
            auto value = executorInstance->ExecuteLuaCallback(packArgs);
            LuaInnerCore::PushUnknownValue<ReturnValueType, std::is_pointer<ReturnValueType>::value>::Do(state, value);
            return 1;
         }
      };

      template <typename ILuaExecutor_t, typename ArgsPack_t>
      struct LuaCallbackReturnValue<ILuaExecutor_t, ArgsPack_t, void>
      {
         static int PushToLua(lua_State* state, ILuaExecutor_t* executorInstance, ArgsPack_t& packArgs)
         {
            executorInstance->ExecuteLuaCallback(packArgs);
            return 0;
         }
      };

      /*------------ Inner Core  --------------*/
   }

   template <typename GlobalVariableType>
   struct LuaGetGlobal;

   template <>
   struct LuaGetGlobal<int64_t>
   {
      static int64_t Value(const LuaWrapper& instanceWrapper, const std::string& variableName, const int32_t stackIndex)
      {
         LuaInnerCore::LuaGetGlobalBase::GetGlobal(instanceWrapper, variableName);
         return LuaInnerCore::GetValue<int64_t>::Value(instanceWrapper, stackIndex);
      }
   };

   template <>
   struct LuaGetGlobal<double>
   {
      static double Value(const LuaWrapper& instanceWrapper, const std::string& variableName, const int32_t stackIndex)
      {
         LuaInnerCore::LuaGetGlobalBase::GetGlobal(instanceWrapper, variableName);
         return LuaInnerCore::GetValue<double>::Value(instanceWrapper, stackIndex);
      }
   };

   template <>
   struct LuaGetGlobal<float>
   {
      static double Value(const LuaWrapper& instanceWrapper, const std::string& variableName, const int32_t stackIndex)
      {
         LuaInnerCore::LuaGetGlobalBase::GetGlobal(instanceWrapper, variableName);
         return LuaInnerCore::GetValue<float>::Value(instanceWrapper, stackIndex);
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

         LuaInnerCore::IterateFunctionArgs<TArgs...>::PushArg(instanceWrapper, std::forward<TArgs>(args)...);

         static constexpr size_t argsCount = sizeof...(args);
         LuaInnerCore::CheckLuaExecution::Do(instanceWrapper, lua_pcall(instanceWrapper.GetState(), argsCount, 1, /*error handling in lua*/0));

         return LuaInnerCore::GetValue<RetType>::Value(instanceWrapper, -1);
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

         LuaInnerCore::IterateFunctionArgs<TArgs...>::PushArg(instanceWrapper, std::forward<TArgs>(args)...);

         static constexpr size_t argsCount = sizeof...(args);
         LuaInnerCore::CheckLuaExecution::Do(instanceWrapper, lua_pcall(instanceWrapper.GetState(), argsCount, 0, /*error handling in lua*/0));
      }
   };

   template <typename ILuaExecutor, typename FunctorType>
   struct LuaRegisterCallback;

   template <typename ILuaExecutor, typename RetType, typename... Args>
   struct LuaRegisterCallback<ILuaExecutor, RetType(Args...)>
   {
      using type = LuaRegisterCallback<ILuaExecutor, RetType(Args...)>;
      using args_t = std::tuple<Args...>;

      static void Rigister(const LuaWrapper& instanceWrapper, const std::string& functionName)
      {
         lua_register(instanceWrapper.GetState(), functionName.c_str(), type::Wrapper);
      }

   private:
      static int Wrapper(lua_State* state)
      {
         static constexpr size_t argsCount = sizeof...(Args);
         assert(lua_gettop(state) != argsCount);

         ILuaExecutor* instance = static_cast<ILuaExecutor*>(lua_touserdata(state, 1));
         assert(instance);

         args_t parameterPackInstance;
         LuaInnerCore::CollectArgsFromLuaHostInvoke<args_t, argsCount>::Collect(state, parameterPackInstance);

         return LuaInnerCore::LuaCallbackReturnValue<ILuaExecutor, args_t, RetType>::PushToLua(state, instance, parameterPackInstance);
      }
   };
}