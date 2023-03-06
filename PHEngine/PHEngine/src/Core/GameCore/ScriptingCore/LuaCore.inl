#pragma once
#include <string>
#include <stdint.h>
#include <tuple>
#include <type_traits>
#include <glm/vec3.hpp>
#include <glm/ext/quaternion_float.hpp>
#include <algorithm>
#include <utility>
#include <any>
#include <functional>

extern "C"
{
#include <lua/lua.h>
#include <lua/lualib.h>
#include <lua/lauxlib.h>
}

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/ScriptingCore/LuaScriptExecutors/LuaScriptExecutorBase.h"
#include "LuaWrapper.h"
#include "LuaHelper.h"

#ifdef _WIN32
#define FORCEINLINE __forceinline
#elif __linux__
#define FORCEINLINE __attribute__((always_inline))
#endif

using namespace EngineCore;

namespace EngineCore
{
   namespace Scripts
   {
      struct LuaTableBase
      {
      };

      template <typename... Args>
      struct LuaTable : public LuaTableBase,
                        public std::tuple<Args...>
      {
      };

      namespace LuaInnerCore
      {
         /*------------ Inner Core  --------------*/

         template <typename ArgType>
         struct LuaValuePusher;

         template <>
         struct LuaValuePusher<void *>
         {
            FORCEINLINE static void Push(lua_State *state, void *value)
            {
               lua_pushlightuserdata(state, value);
            }
         };

         template <>
         struct LuaValuePusher<std::string>
         {
            FORCEINLINE static void Push(lua_State *state, const std::string &value)
            {
               lua_pushstring(state, value.c_str());
            }
         };

         template <>
         struct LuaValuePusher<float>
         {
            FORCEINLINE static void Push(lua_State *state, const float &value)
            {
               lua_pushnumber(state, value);
            }
         };

         template <>
         struct LuaValuePusher<int32_t>
         {
            FORCEINLINE static void Push(lua_State *state, const int32_t &value)
            {
               lua_pushinteger(state, value);
            }
         };

         template <>
         struct LuaValuePusher<bool>
         {
            FORCEINLINE static void Push(lua_State *state, const bool &value)
            {
               lua_pushboolean(state, value);
            }
         };

         template <>
         struct LuaValuePusher<double>
         {
            FORCEINLINE static void Push(lua_State *state, const double &value)
            {
               lua_pushnumber(state, value);
            }
         };

         template <>
         struct LuaValuePusher<int64_t>
         {
            FORCEINLINE static void Push(lua_State *state, const int64_t &value)
            {
               lua_pushinteger(state, value);
            }
         };

         template <typename ArgType>
         inline typename std::enable_if<std::is_pointer<ArgType>::value>::type PushValueToLua(lua_State *state, const ArgType &value)
         {
            LuaValuePusher<void *>::Push(state, (void *)value);
         }

         template <typename ArgType>
         inline typename std::enable_if<!std::is_pointer<ArgType>::value>::type PushValueToLua(lua_State *state, const ArgType &value)
         {
            LuaValuePusher<ArgType>::Push(state, value);
         }

         template <typename... Args>
         struct LuaMultipleValuesPusher;

         template <typename Arg, typename... Args>
         struct LuaMultipleValuesPusher<Arg, Args...>
         {
            FORCEINLINE static void Push(const LuaWrapper &instanceWrapper, Arg &&arg, Args &&...args)
            {
               LuaValuePusher<typename std::decay<Arg>::type>::Push(instanceWrapper.GetState(), std::forward<Arg>(arg));
               LuaMultipleValuesPusher<Args...>::Push(instanceWrapper, std::forward<Args>(args)...);
            }
         };

         template <>
         struct LuaMultipleValuesPusher<>
         {
            FORCEINLINE static void Push(const LuaWrapper &instanceWrapper) {}
         };

         template <typename VariableType>
         struct GetLuaValue
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

         template <>
         struct GetLuaValue<std::string>
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
         struct GetLuaValue<int64_t>
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
         struct GetLuaValue<bool>
         {
         public:
            FORCEINLINE static bool Value(const LuaWrapper &instanceWrapper, int32_t &stackIndex)
            {
               return Inner_Value(instanceWrapper.GetState(), stackIndex);
            }

            FORCEINLINE static bool Value(lua_State *state, int32_t &stackIndex)
            {
               return Inner_Value(state, stackIndex);
            }

         private:
            FORCEINLINE static bool Inner_Value(lua_State *state, int32_t &stackIndex)
            {
               const int32_t currentStackIndex = stackIndex--;
               assert(lua_isboolean(state, currentStackIndex));
               return lua_toboolean(state, currentStackIndex);
            }
         };

         template <>
         struct GetLuaValue<double>
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
         struct GetLuaValue<int32_t>
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
         struct GetLuaValue<float>
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
         struct GetLuaValue<void>
         {
            FORCEINLINE static void Value(const LuaWrapper &instanceWrapper, int32_t &stackIndex)
            {
            }
         };

         template <>
         struct GetLuaValue<glm::quat>
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
               const float w = GetLuaValue<float>::Value(state, stackIndex);
               const float z = GetLuaValue<float>::Value(state, stackIndex);
               const float y = GetLuaValue<float>::Value(state, stackIndex);
               const float x = GetLuaValue<float>::Value(state, stackIndex);
               return glm::quat(w, x, y, z);
            }
         };

         template <>
         struct GetLuaValue<glm::vec4>
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
               const float w = GetLuaValue<float>::Value(state, stackIndex);
               const float z = GetLuaValue<float>::Value(state, stackIndex);
               const float y = GetLuaValue<float>::Value(state, stackIndex);
               const float x = GetLuaValue<float>::Value(state, stackIndex);
               return glm::vec4(x, y, z, w);
            }
         };

         template <>
         struct GetLuaValue<glm::ivec4>
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
               const int32_t w = GetLuaValue<int32_t>::Value(state, stackIndex);
               const int32_t z = GetLuaValue<int32_t>::Value(state, stackIndex);
               const int32_t y = GetLuaValue<int32_t>::Value(state, stackIndex);
               const int32_t x = GetLuaValue<int32_t>::Value(state, stackIndex);
               return glm::ivec4(x, y, z, w);
            }
         };

         template <>
         struct GetLuaValue<glm::vec3>
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
               const float z = GetLuaValue<float>::Value(state, stackIndex);
               const float y = GetLuaValue<float>::Value(state, stackIndex);
               const float x = GetLuaValue<float>::Value(state, stackIndex);
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
         struct GetLuaArgsPack
         {
            using arg_type = typename std::tuple_element<argsCount - 1, tuple_type>::type;

            FORCEINLINE static void Collect(lua_State *state, tuple_type &params, int32_t &stackIndex)
            {
               std::get<argsCount - 1>(params) = GetLuaValue<arg_type>::Value(state, stackIndex);
               GetLuaArgsPack<tuple_type, argsCount - 1>::Collect(state, params, stackIndex);
            }
         };

         template <typename tuple_type>
         struct GetLuaArgsPack<tuple_type, 0>
         {
            FORCEINLINE static void Collect(lua_State *state, tuple_type &params, int32_t &stackIndex) {}
         };

         struct LuaGetGlobalBase
         {
            FORCEINLINE static void GetGlobal(const LuaWrapper &instanceWrapper, const std::string &variableName)
            {
               lua_getglobal(instanceWrapper.GetState(), variableName.c_str());
            }
         };

         template <typename FunctorType, typename ArgsPack_t, typename ReturnValueType>
         struct LuaCallbackInvoker
         {
            FORCEINLINE static int Invoke(lua_State *state, void *ownerPtr, const uint64_t funcHash, const ArgsPack_t &packArgs)
            {
               LuaScriptExecutorBase *baseExecutorInstance = reinterpret_cast<LuaScriptExecutorBase *>(ownerPtr);
               const auto functor_any = baseExecutorInstance->GetFunctorAny(funcHash);
               const auto &functor = std::any_cast<FunctorType>(functor_any);
               const auto retValue = functor(packArgs);
               PushValueToLua<ReturnValueType>(state, retValue);
               return 1;
            }
         };

         template <typename FunctorType, typename ArgsPack_t>
         struct LuaCallbackInvoker<FunctorType, ArgsPack_t, void>
         {
            FORCEINLINE static int Invoke(lua_State *state, void *ownerPtr, const uint64_t funcHash, const ArgsPack_t &packArgs)
            {
               LuaScriptExecutorBase *baseExecutorInstance = reinterpret_cast<LuaScriptExecutorBase *>(ownerPtr);
               const auto functor_any = baseExecutorInstance->GetFunctorAny(funcHash);
               const auto &functor = std::any_cast<FunctorType>(functor_any);
               functor(packArgs);
               return 0;
            }
         };

         template <typename T>
         struct LuaArgsCountForType
         {
            static constexpr int32_t value = 1;
         };
         template <>
         struct LuaArgsCountForType<glm::vec3>
         {
            static constexpr int32_t value = 3;
         };
         template <>
         struct LuaArgsCountForType<glm::vec4>
         {
            static constexpr int32_t value = 4;
         };
         template <>
         struct LuaArgsCountForType<glm::ivec4>
         {
            static constexpr int32_t value = 4;
         };
         template <>
         struct LuaArgsCountForType<glm::quat>
         {
            static constexpr int32_t value = 4;
         };

         template <typename tuple_t, int32_t currentIndex>
         struct LuaRealArgsCounter
         {
            static constexpr int32_t value = LuaArgsCountForType<typename std::tuple_element<currentIndex, tuple_t>::type>::value +
                                             LuaRealArgsCounter<tuple_t, currentIndex - 1>::value;
         };

         template <typename tuple_t>
         struct LuaRealArgsCounter<tuple_t, -1>
         {
            static constexpr int32_t value = 0;
         };
         /*------------ Inner Core  --------------*/
      }

      template <typename GlobalVariableType>
      struct GetLuaGlobalVariable;

      template <>
      struct GetLuaGlobalVariable<std::string>
      {
         FORCEINLINE static std::string Value(const LuaWrapper &instanceWrapper, const std::string &variableName, int32_t stackIndex)
         {
            LuaInnerCore::LuaGetGlobalBase::GetGlobal(instanceWrapper, variableName);
            return LuaInnerCore::GetLuaValue<std::string>::Value(instanceWrapper, stackIndex);
         }
      };

      template <>
      struct GetLuaGlobalVariable<int64_t>
      {
         FORCEINLINE static int64_t Value(const LuaWrapper &instanceWrapper, const std::string &variableName, int32_t stackIndex)
         {
            LuaInnerCore::LuaGetGlobalBase::GetGlobal(instanceWrapper, variableName);
            return LuaInnerCore::GetLuaValue<int64_t>::Value(instanceWrapper, stackIndex);
         }
      };

      template <>
      struct GetLuaGlobalVariable<double>
      {
         FORCEINLINE static double Value(const LuaWrapper &instanceWrapper, const std::string &variableName, int32_t stackIndex)
         {
            LuaInnerCore::LuaGetGlobalBase::GetGlobal(instanceWrapper, variableName);
            return LuaInnerCore::GetLuaValue<double>::Value(instanceWrapper, stackIndex);
         }
      };

      template <>
      struct GetLuaGlobalVariable<float>
      {
         FORCEINLINE static float Value(const LuaWrapper &instanceWrapper, const std::string &variableName, int32_t stackIndex)
         {
            LuaInnerCore::LuaGetGlobalBase::GetGlobal(instanceWrapper, variableName);
            return LuaInnerCore::GetLuaValue<float>::Value(instanceWrapper, stackIndex);
         }
      };

      template <typename FunctionType>
      struct LuaFunctionInvoker;

      template <typename RetType, typename... Args>
      struct LuaFunctionInvoker<RetType(Args...)>
      {

         template <typename... TArgs>
         FORCEINLINE static RetType Invoke(const LuaWrapper &instanceWrapper, const std::string &functionName, TArgs &&...args)
         {
            lua_getglobal(instanceWrapper.GetState(), functionName.c_str());
            assert(lua_isfunction(instanceWrapper.GetState(), -1));

            LuaInnerCore::LuaMultipleValuesPusher<TArgs...>::Push(instanceWrapper, std::forward<TArgs>(args)...);

            static constexpr size_t argsCount = sizeof...(args);
            HasLuaError(instanceWrapper, lua_pcall(instanceWrapper.GetState(), argsCount, 1, /*error handling in lua*/ 0));

            int32_t stackIndex = -1;
            return LuaInnerCore::GetLuaValue<typename std::decay<RetType>::type>::Value(instanceWrapper, stackIndex);
         }
      };

      template <typename... Args>
      struct LuaFunctionInvoker<void(Args...)>
      {

         template <typename... TArgs>
         FORCEINLINE static void Invoke(const LuaWrapper &instanceWrapper, const std::string &functionName, TArgs &&...args)
         {
            lua_getglobal(instanceWrapper.GetState(), functionName.c_str());
            assert(lua_isfunction(instanceWrapper.GetState(), -1));

            LuaInnerCore::LuaMultipleValuesPusher<TArgs...>::Push(instanceWrapper, std::forward<TArgs>(args)...);

            static constexpr size_t argsCount = sizeof...(args);
            HasLuaError(instanceWrapper, lua_pcall(instanceWrapper.GetState(), argsCount, 0, /*error handling in lua*/ 0));
         }
      };

      template <uint64_t funcHash, typename FunctorType>
      struct LuaCallbackBinder;

      template <uint64_t funcHash, typename ReturnType, typename... ArgsType>
      struct LuaCallbackBinder<funcHash, ReturnType(ArgsType...)>
      {
         using this_t = LuaCallbackBinder<funcHash, ReturnType(ArgsType...)>;
         using args_t = std::tuple<ArgsType...>;
         using return_t = ReturnType;

         static constexpr auto sFuncHash = funcHash;

         FORCEINLINE static void Bind(const LuaWrapper &luaStateWrapper, const std::string &functionName)
         {
            lua_register(luaStateWrapper.GetState(), functionName.c_str(), this_t::InnerInvokeCallback);
         }

      private:
         FORCEINLINE static int InnerInvokeCallback(lua_State *state)
         {
            using namespace LuaInnerCore;
            assert(lua_gettop(state) != 0); // Check missing host data
            auto ownerPtr = lua_touserdata(state, 1);
            assert(ownerPtr);

            static constexpr size_t argsCount = sizeof...(ArgsType);
            auto topStackIndex = LuaRealArgsCounter<args_t, argsCount - 1>::value + 1; // + 1 because of host data at index 1

            args_t parameterPack;
            GetLuaArgsPack<args_t, argsCount>::Collect(state, parameterPack, topStackIndex);

            return LuaCallbackInvoker<std::function<return_t(args_t)>, args_t, return_t>::Invoke(state, ownerPtr, sFuncHash, parameterPack);
         }
      };
   }
}