#pragma once
#include <string>
#include <stdint.h>
#include <tuple>
#include <type_traits>
#include <glm/vec3.hpp>
#include <glm/ext/quaternion_float.hpp>
#include <algorithm>
#include <utility>

extern "C"
{
#include <lua/lua.h>
#include <lua/lualib.h>
#include <lua/lauxlib.h>
}

#include "Core/CommonCore/Assertion.h"
#include "LuaWrapper.h"

#define FORCEINLINE __attribute__((always_inline))

namespace EngineCore
{
   struct LuaTableBase
   {
   };

   template <typename... Args>
   struct LuaTable : public LuaTableBase,
                     public std::tuple<Args...>
   {
   };

   template <typename = void>
   struct LuaArgDummyPlaceholder
   {
   };

   namespace LuaInnerCore
   {
      /*------------ Inner Core  --------------*/
      struct CheckLuaExecution
      {
         FORCEINLINE static bool Do(const LuaWrapper &instanceWrapper, int32_t luaCallResult)
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

      template <>
      struct PushValue<void *>
      {
         FORCEINLINE static void Do(lua_State *state, void *value)
         {
            lua_pushlightuserdata(state, value);
         }
      };

      template <>
      struct PushValue<std::string>
      {
         FORCEINLINE static void Do(lua_State *state, const std::string &value)
         {
            lua_pushstring(state, value.c_str());
         }
      };

      template <>
      struct PushValue<float>
      {
         FORCEINLINE static void Do(lua_State *state, const float &value)
         {
            lua_pushnumber(state, value);
         }
      };

      template <>
      struct PushValue<int32_t>
      {
         FORCEINLINE static void Do(lua_State *state, const int32_t &value)
         {
            lua_pushinteger(state, value);
         }
      };

      template <>
      struct PushValue<double>
      {
         FORCEINLINE static void Do(lua_State *state, const double &value)
         {
            lua_pushnumber(state, value);
         }
      };

      template <>
      struct PushValue<int64_t>
      {
         FORCEINLINE static void Do(lua_State *state, const int64_t &value)
         {
            lua_pushinteger(state, value);
         }
      };

      template <typename ArgType>
      inline typename std::enable_if<std::is_pointer<ArgType>::value>::type PushTypeValue(lua_State *state, const ArgType &value)
      {
         PushValue<void *>::Do(state, (void *)value);
      }

      template <typename ArgType>
      inline typename std::enable_if<!std::is_pointer<ArgType>::value>::type PushTypeValue(lua_State *state, const ArgType &value)
      {
         PushValue<ArgType>::Do(state, value);
      }

      template <typename... Args>
      struct IterateFunctionArgs;

      template <typename Arg, typename... Args>
      struct IterateFunctionArgs<Arg, Args...>
      {
         FORCEINLINE static void PushArg(const LuaWrapper &instanceWrapper, Arg &&arg, Args &&...args)
         {
            PushValue<typename std::decay<Arg>::type>::Do(instanceWrapper.GetState(), std::forward<Arg>(arg));
            IterateFunctionArgs<Args...>::PushArg(instanceWrapper, std::forward<Args>(args)...);
         }
      };

      template <>
      struct IterateFunctionArgs<>
      {
         FORCEINLINE static void PushArg(const LuaWrapper &instanceWrapper)
         {
            return;
         }
      };

      template <typename VariableType>
      struct GetValue
      {
         FORCEINLINE static VariableType Value(const LuaWrapper &instanceWrapper, int32_t &stackIndex)
         {
            return Inner_Value(instanceWrapper.GetState(), stackIndex);
         }

         FORCEINLINE static VariableType Value(lua_State *state, int32_t &stackIndex)
         {
            return Inner_Value(state, stackIndex);
         }

      private:
         FORCEINLINE static typename std::enable_if<std::is_pointer<VariableType>::value, VariableType>::type Inner_Value(lua_State *state, int32_t &stackIndex)
         {
            const int32_t currentStackIndex = stackIndex--;
            return (VariableType)lua_touserdata(state, currentStackIndex);
         }
      };

      template <typename U>
      struct GetValue<LuaArgDummyPlaceholder<U>>
      {
      public:
         FORCEINLINE static LuaArgDummyPlaceholder<U> Value(const LuaWrapper &instanceWrapper, int32_t &stackIndex)
         {
            return Inner_Value(instanceWrapper.GetState(), stackIndex);
         }

         FORCEINLINE static LuaArgDummyPlaceholder<U> Value(lua_State *state, int32_t &stackIndex)
         {
            return Inner_Value(state, stackIndex);
         }

      private:
         FORCEINLINE static LuaArgDummyPlaceholder<U> Inner_Value(lua_State *state, int32_t &stackIndex)
         {
            return LuaArgDummyPlaceholder<U>();
         }
      };

      template <>
      struct GetValue<std::string>
      {
      public:
         static std::string Value(const LuaWrapper &instanceWrapper, int32_t &stackIndex)
         {
            return Inner_Value(instanceWrapper.GetState(), stackIndex);
         }

         static std::string Value(lua_State *state, int32_t &stackIndex)
         {
            return Inner_Value(state, stackIndex);
         }

      private:
         FORCEINLINE static std::string Inner_Value(lua_State *state, int32_t &stackIndex)
         {
            const int32_t currentStackIndex = stackIndex--;
            assert(lua_isstring(state, currentStackIndex));
            return lua_tostring(state, currentStackIndex);
         }
      };

      template <>
      struct GetValue<int64_t>
      {
      public:
         FORCEINLINE static int64_t Value(const LuaWrapper &instanceWrapper, int32_t &stackIndex)
         {
            return Inner_Value(instanceWrapper.GetState(), stackIndex);
         }

         FORCEINLINE static int64_t Value(lua_State *state, int32_t &stackIndex)
         {
            return Inner_Value(state, stackIndex);
         }

      private:
         FORCEINLINE static int64_t Inner_Value(lua_State *state, int32_t &stackIndex)
         {
            const int32_t currentStackIndex = stackIndex--;
            assert(lua_isinteger(state, currentStackIndex));
            return lua_tointeger(state, currentStackIndex);
         }
      };

      template <>
      struct GetValue<double>
      {
      public:
         FORCEINLINE static double Value(const LuaWrapper &instanceWrapper, int32_t &stackIndex)
         {
            return Inner_Value(instanceWrapper.GetState(), stackIndex);
         }

         FORCEINLINE static double Value(lua_State *state, int32_t &stackIndex)
         {
            return Inner_Value(state, stackIndex);
         }

      private:
         FORCEINLINE static double Inner_Value(lua_State *state, int32_t &stackIndex)
         {
            const int32_t currentStackIndex = stackIndex--;
            assert(lua_isnumber(state, currentStackIndex));
            return lua_tonumber(state, currentStackIndex);
         }
      };

      template <>
      struct GetValue<int32_t>
      {
      public:
         FORCEINLINE static int32_t Value(const LuaWrapper &instanceWrapper, int32_t &stackIndex)
         {
            return Inner_Value(instanceWrapper.GetState(), stackIndex);
         }

         FORCEINLINE static int32_t Value(lua_State *state, int32_t &stackIndex)
         {
            return Inner_Value(state, stackIndex);
         }

      private:
         FORCEINLINE static int32_t Inner_Value(lua_State *state, int32_t &stackIndex)
         {
            const int32_t currentStackIndex = stackIndex--;
            assert(lua_isinteger(state, currentStackIndex));
            return (int32_t)lua_tointeger(state, currentStackIndex);
         }
      };

      template <>
      struct GetValue<float>
      {
      public:
         FORCEINLINE static float Value(const LuaWrapper &instanceWrapper, int32_t &stackIndex)
         {
            return Inner_Value(instanceWrapper.GetState(), stackIndex);
         }

         FORCEINLINE static float Value(lua_State *state, int32_t &stackIndex)
         {
            return Inner_Value(state, stackIndex);
         }

      private:
         FORCEINLINE static float Inner_Value(lua_State *state, int32_t &stackIndex)
         {
            const int32_t currentStackIndex = stackIndex--;
            assert(lua_isnumber(state, currentStackIndex));
            return (float)lua_tonumber(state, currentStackIndex);
         }
      };

      template <>
      struct GetValue<void>
      {
         FORCEINLINE static void Value(const LuaWrapper &instanceWrapper, int32_t &stackIndex)
         {
         }
      };

      template <>
      struct GetValue<glm::quat>
      {
      public:
         FORCEINLINE static glm::quat Value(const LuaWrapper &instanceWrapper, int32_t &stackIndex)
         {
            return Inner_Value(instanceWrapper.GetState(), stackIndex);
         }

         FORCEINLINE static glm::quat Value(lua_State *state, int32_t &stackIndex)
         {
            return Inner_Value(state, stackIndex);
         }

      private:
         FORCEINLINE static glm::quat Inner_Value(lua_State *state, int32_t &stackIndex)
         {
            // direction is reversed because stackIndex is decreasing
            const float w = GetValue<float>::Value(state, stackIndex);
            const float z = GetValue<float>::Value(state, stackIndex);
            const float y = GetValue<float>::Value(state, stackIndex);
            const float x = GetValue<float>::Value(state, stackIndex);
            return glm::quat(w, x, y, z);
         }
      };

      template <>
      struct GetValue<glm::vec4>
      {
      public:
         FORCEINLINE static glm::vec4 Value(const LuaWrapper &instanceWrapper, int32_t &stackIndex)
         {
            return Inner_Value(instanceWrapper.GetState(), stackIndex);
         }

         FORCEINLINE static glm::vec4 Value(lua_State *state, int32_t &stackIndex)
         {
            return Inner_Value(state, stackIndex);
         }

      private:
         FORCEINLINE static glm::vec4 Inner_Value(lua_State *state, int32_t &stackIndex)
         {
            // direction is reversed because stackIndex is decreasing
            const float w = GetValue<float>::Value(state, stackIndex);
            const float z = GetValue<float>::Value(state, stackIndex);
            const float y = GetValue<float>::Value(state, stackIndex);
            const float x = GetValue<float>::Value(state, stackIndex);
            return glm::vec4(x, y, z, w);
         }
      };

      template <>
      struct GetValue<glm::ivec4>
      {
      public:
         FORCEINLINE static glm::ivec4 Value(const LuaWrapper &instanceWrapper, int32_t &stackIndex)
         {
            return Inner_Value(instanceWrapper.GetState(), stackIndex);
         }

         FORCEINLINE static glm::ivec4 Value(lua_State *state, int32_t &stackIndex)
         {
            return Inner_Value(state, stackIndex);
         }

      private:
         FORCEINLINE static glm::ivec4 Inner_Value(lua_State *state, int32_t &stackIndex)
         {
            // direction is reversed because stackIndex is decreasing
            const int32_t w = GetValue<int32_t>::Value(state, stackIndex);
            const int32_t z = GetValue<int32_t>::Value(state, stackIndex);
            const int32_t y = GetValue<int32_t>::Value(state, stackIndex);
            const int32_t x = GetValue<int32_t>::Value(state, stackIndex);
            return glm::ivec4(x, y, z, w);
         }
      };

      template <>
      struct GetValue<glm::vec3>
      {
      public:
         FORCEINLINE static glm::vec3 Value(const LuaWrapper &instanceWrapper, int32_t &stackIndex)
         {
            return Inner_Value(instanceWrapper.GetState(), stackIndex);
         }

         FORCEINLINE static glm::vec3 Value(lua_State *state, int32_t &stackIndex)
         {
            return Inner_Value(state, stackIndex);
         }

      private:
         FORCEINLINE static glm::vec3 Inner_Value(lua_State *state, int32_t &stackIndex)
         {
            // direction is reversed because stackIndex is decreasing
            const float z = GetValue<float>::Value(state, stackIndex);
            const float y = GetValue<float>::Value(state, stackIndex);
            const float x = GetValue<float>::Value(state, stackIndex);
            return glm::vec3(x, y, z);
         }
      };

      template <size_t LuaTableParamCount, typename tuple_type, typename LuaTableType>
      struct GetLuaTableValue;

      template <size_t LuaTableParamCount, typename tuple_type, typename... Args>
      struct GetLuaTableValue<LuaTableParamCount, tuple_type, LuaTable<Args...>>
      {
         FORCEINLINE static void Value(const LuaWrapper &instanceWrapper, const int32_t stackIndex)
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

         FORCEINLINE static void Collect(lua_State *state, tuple_type &params, int32_t &stackIndex)
         {
            std::get<argsCount - 1>(params) = GetValue<arg_type>::Value(state, stackIndex);
            CollectArgsFromLuaHostInvoke<tuple_type, argsCount - 1>::Collect(state, params, stackIndex);
         }
      };

      template <typename tuple_type>
      struct CollectArgsFromLuaHostInvoke<tuple_type, 0>
      {
         FORCEINLINE static void Collect(lua_State *state, tuple_type &params, int32_t &stackIndex)
         {
         }
      };

      struct LuaGetGlobalBase
      {
         FORCEINLINE static void GetGlobal(const LuaWrapper &instanceWrapper, const std::string &variableName)
         {
            lua_getglobal(instanceWrapper.GetState(), variableName.c_str());
         }
      };

      template <typename ILuaExecutor_t, typename ArgsPack_t, typename ReturnValueType>
      struct LuaCallbackReturnValue
      {
         FORCEINLINE static int PushToLua(lua_State *state, ILuaExecutor_t *executorInstance, ArgsPack_t &packArgs)
         {
            const auto value = executorInstance->ExecuteLuaCallback(packArgs);
            LuaInnerCore::PushTypeValue<ReturnValueType>(state, value);
            return 1;
         }
      };

      template <typename ILuaExecutor_t, typename ArgsPack_t>
      struct LuaCallbackReturnValue<ILuaExecutor_t, ArgsPack_t, void>
      {
         FORCEINLINE static int PushToLua(lua_State *state, ILuaExecutor_t *executorInstance, ArgsPack_t &packArgs)
         {
            executorInstance->ExecuteLuaCallback(packArgs);
            return 0;
         }
      };

      template <typename T>
      struct LuaArgsCountForType
      {
         enum
         {
            value = 1
         };
      };
      template <typename U>
      struct LuaArgsCountForType<LuaArgDummyPlaceholder<U>>
      {
         enum
         {
            value = 0
         };
      };
      template <>
      struct LuaArgsCountForType<glm::vec3>
      {
         enum
         {
            value = 3
         };
      };
      template <>
      struct LuaArgsCountForType<glm::vec4>
      {
         enum
         {
            value = 4
         };
      };
      template <>
      struct LuaArgsCountForType<glm::ivec4>
      {
         enum
         {
            value = 4
         };
      };
      template <>
      struct LuaArgsCountForType<glm::quat>
      {
         enum
         {
            value = 4
         };
      };

      template <typename tuple_t, int32_t currentIndex>
      struct CollectLuaArgsCount_Inner
      {
         using arg_t = typename std::tuple_element<currentIndex, tuple_t>::type;
         enum
         {
            value = LuaArgsCountForType<arg_t>::value + CollectLuaArgsCount_Inner<tuple_t, currentIndex - 1>::value
         };
      };

      template <typename tuple_t>
      struct CollectLuaArgsCount_Inner<tuple_t, -1>
      {
         enum
         {
            value = 0
         };
      };
      /*------------ Inner Core  --------------*/
   }

   template <typename GlobalVariableType>
   struct LuaGetGlobal;

   template <>
   struct LuaGetGlobal<std::string>
   {
      FORCEINLINE static std::string Value(const LuaWrapper &instanceWrapper, const std::string &variableName, int32_t stackIndex)
      {
         LuaInnerCore::LuaGetGlobalBase::GetGlobal(instanceWrapper, variableName);
         return LuaInnerCore::GetValue<std::string>::Value(instanceWrapper, stackIndex);
      }
   };

   template <>
   struct LuaGetGlobal<int64_t>
   {
      FORCEINLINE static int64_t Value(const LuaWrapper &instanceWrapper, const std::string &variableName, int32_t stackIndex)
      {
         LuaInnerCore::LuaGetGlobalBase::GetGlobal(instanceWrapper, variableName);
         return LuaInnerCore::GetValue<int64_t>::Value(instanceWrapper, stackIndex);
      }
   };

   template <>
   struct LuaGetGlobal<double>
   {
      FORCEINLINE static double Value(const LuaWrapper &instanceWrapper, const std::string &variableName, int32_t stackIndex)
      {
         LuaInnerCore::LuaGetGlobalBase::GetGlobal(instanceWrapper, variableName);
         return LuaInnerCore::GetValue<double>::Value(instanceWrapper, stackIndex);
      }
   };

   template <>
   struct LuaGetGlobal<float>
   {
      FORCEINLINE static float Value(const LuaWrapper &instanceWrapper, const std::string &variableName, int32_t stackIndex)
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
      FORCEINLINE static RetType Call(const LuaWrapper &instanceWrapper, const std::string &functionName, TArgs &&...args)
      {
         lua_getglobal(instanceWrapper.GetState(), functionName.c_str());
         assert(lua_isfunction(instanceWrapper.GetState(), -1));

         LuaInnerCore::IterateFunctionArgs<TArgs...>::PushArg(instanceWrapper, std::forward<TArgs>(args)...);

         static constexpr size_t argsCount = sizeof...(args);
         LuaInnerCore::CheckLuaExecution::Do(instanceWrapper, lua_pcall(instanceWrapper.GetState(), argsCount, 1, /*error handling in lua*/ 0));

         int32_t stackIndex = -1;
         return LuaInnerCore::GetValue<typename std::decay<RetType>::type>::Value(instanceWrapper, stackIndex);
      }
   };

   template <typename... Args>
   struct LuaFunction<void(Args...)>
   {

      template <typename... TArgs>
      FORCEINLINE static void Call(const LuaWrapper &instanceWrapper, const std::string &functionName, TArgs &&...args)
      {
         lua_getglobal(instanceWrapper.GetState(), functionName.c_str());
         assert(lua_isfunction(instanceWrapper.GetState(), -1));

         LuaInnerCore::IterateFunctionArgs<TArgs...>::PushArg(instanceWrapper, std::forward<TArgs>(args)...);

         static constexpr size_t argsCount = sizeof...(args);
         LuaInnerCore::CheckLuaExecution::Do(instanceWrapper, lua_pcall(instanceWrapper.GetState(), argsCount, 0, /*error handling in lua*/ 0));
      }
   };

   template <typename ILuaExecutor, typename FunctorType>
   struct LuaRegisterCallback;

   template <typename ILuaExecutor, typename RetType, typename... Args>
   struct LuaRegisterCallback<ILuaExecutor, RetType(Args...)>
   {
      using type = LuaRegisterCallback<ILuaExecutor, RetType(Args...)>;
      using args_t = std::tuple<Args...>;

      FORCEINLINE static void Register(const LuaWrapper &instanceWrapper, const std::string &functionName)
      {
         lua_register(instanceWrapper.GetState(), functionName.c_str(), type::Wrapper);
      }

   private:
      FORCEINLINE static int Wrapper(lua_State *state)
      {
         static constexpr size_t argsCount = sizeof...(Args);
         assert((lua_gettop(state) != 0, "Missing host data"));

         ILuaExecutor *instance = static_cast<ILuaExecutor *>(lua_touserdata(state, 1));
         assert(instance);

         int32_t stackIndex = LuaInnerCore::CollectLuaArgsCount_Inner<args_t, argsCount - 1>::value + 1; // + 1 because of host data at index 1

         args_t parameterPackInstance;
         LuaInnerCore::CollectArgsFromLuaHostInvoke<args_t, argsCount>::Collect(state, parameterPackInstance, stackIndex);

         return LuaInnerCore::LuaCallbackReturnValue<ILuaExecutor, args_t, RetType>::PushToLua(state, instance, parameterPackInstance);
      }
   };
}