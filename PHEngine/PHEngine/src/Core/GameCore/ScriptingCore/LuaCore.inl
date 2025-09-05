/**
 * @file LuaCore.inl
 * @brief Provides core utilities for integrating C++ types and functions with Lua scripts.
 *
 * This file contains templates and helper structures for pushing and retrieving values
 * between C++ and Lua, invoking Lua functions from C++, and binding C++ callbacks to Lua.
 * It supports various types including primitives, std::string, glm vectors/quaternions,
 * and user-defined types. It also provides mechanisms for handling Lua tables and
 * global variables, as well as error handling and assertion checks.
 *
 * Main components:
 * - ValuePusher<T>: Pushes C++ values of type T onto the Lua stack.
 * - PushValueToLua: Type-deduced helper for pushing values to Lua.
 * - ChainValuesPusher<Ts...>: Pushes multiple values to Lua in order.
 * - GetValue<T>: Retrieves C++ values of type T from the Lua stack.
 * - GetArgsPack: Collects multiple arguments from the Lua stack into a tuple.
 * - CallbackInvoker: Invokes C++ functors from Lua, handling return values.
 * - LuaFunctionInvoker: Invokes Lua functions from C++, handling arguments and return values.
 * - LuaCallbackBinder: Binds C++ functions as Lua-callable callbacks.
 * - GetLuaGlobalVariable: Retrieves global Lua variables as C++ types.
 * - LuaArgsCountForType<T>: Specifies how many Lua stack values a type occupies.
 * - ArgsCounter: Computes the total number of Lua stack values for a tuple of types.
 *
 * Platform-specific FORCEINLINE macro is defined for function inlining.
 *
 * Dependencies:
 * - GLM for vector and quaternion types.
 * - Lua C API.
 * - EngineCore internal modules (Assertion, LoggerExtension, LuaScriptExecutorBase, LuaHelper, LuaWrapper).
 *
 * @note This file is intended for internal use within the EngineCore::Scripts namespace.
 */
#pragma once
#include <glm/ext/quaternion_float.hpp>
#include <glm/vec3.hpp>
#include <stdint.h>

#include <algorithm>
#include <any>
#include <functional>
#include <tuple>
#include <type_traits>

extern "C" {
#include <lua/lauxlib.h>
#include <lua/lua.h>
#include <lua/lualib.h>
}

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/ScriptingCore/LuaScriptExecutors/LuaScriptExecutorBase.h"
#include "LuaHelper.h"
#include "LuaWrapper.h"

#ifdef _WIN32
#define FORCEINLINE __forceinline
#elif __linux__
#define FORCEINLINE __attribute__((always_inline))
#endif

using namespace EngineCore;

namespace EngineCore::Scripts {
namespace LuaInnerCore {
/*------------ Inner Core  --------------*/

template<typename ArgType>
struct ValuePusher {

    template <typename T>
    FORCEINLINE static void Push(lua_State* state, T&& value)
    {
        using bareType_t = std::decay<T>::type;
        if constexpr (std::is_integral_v<bareType_t>) {
            lua_pushinteger(state, value);
        } else if constexpr (std::is_floating_point_v<bareType_t>) {
            lua_pushnumber(state, value);
        } else if constexpr (std::is_same_v<std::string, bareType_t>) {
            lua_pushstring(state, value.c_str());
        } else if constexpr (std::is_same_v<bool, bareType_t>) {
            lua_pushboolean(state, value);
        } else if constexpr (std::is_same_v<void*, bareType_t>) {
            lua_pushlightuserdata(state, value);
        }
    }
};

template<typename... Args>
struct ChainValuesPusher;

template<typename Arg, typename... Args>
struct ChainValuesPusher<Arg, Args...> {
    FORCEINLINE static void Push(const LuaWrapper& instanceWrapper, Arg&& arg, Args&&... args)
    {
        ValuePusher<typename std::decay<Arg>::type>::Push(instanceWrapper.GetState(), std::forward<Arg>(arg));
        ChainValuesPusher<Args...>::Push(instanceWrapper, std::forward<Args>(args)...);
    }
};

template<>
struct ChainValuesPusher<> {
    FORCEINLINE static void Push(const LuaWrapper& instanceWrapper)
    {
    }
};

template<typename VariableType>
struct GetValue {
    FORCEINLINE static VariableType Value(const LuaWrapper& instanceWrapper, int32_t& stackIndex)
    {
        return CallImpl<VariableType>(instanceWrapper.GetState(), stackIndex);
    }

    FORCEINLINE static VariableType Value(lua_State* state, int32_t& stackIndex)
    {
        return CallImpl<VariableType>(state, stackIndex);
    }

private:
    template<typename ArgType>
    FORCEINLINE static ArgType CallImpl(lua_State* state, int32_t& stackIndex)
    {
        if constexpr (std::is_same_v<ArgType, void*>) {
            const int32_t currentStackIndex = stackIndex--;
            return (ArgType)lua_touserdata(state, currentStackIndex);
        } else if constexpr (std::is_same_v<ArgType, std::string>) {
            const int32_t currentStackIndex = stackIndex--;
            assert(lua_isstring(state, currentStackIndex));
            return lua_tostring(state, currentStackIndex);
        } else if constexpr (std::is_integral_v<ArgType>) {
            const int32_t currentStackIndex = stackIndex--;
            assert(lua_isinteger(state, currentStackIndex));
            return lua_tointeger(state, currentStackIndex);
        } else if constexpr (std::is_floating_point_v<ArgType>) {
            const int32_t currentStackIndex = stackIndex--;
            assert(lua_isnumber(state, currentStackIndex));
            return lua_tonumber(state, currentStackIndex);
        } else if constexpr (std::is_same_v<ArgType, bool>) {
            const int32_t currentStackIndex = stackIndex--;
            assert(lua_isboolean(state, currentStackIndex));
            return lua_toboolean(state, currentStackIndex);
        } else if constexpr (std::is_same_v<ArgType, glm::vec4>) {
            // direction is reversed because stackIndex is decreasing
            const float w = GetValue<float>::Value(state, stackIndex);
            const float z = GetValue<float>::Value(state, stackIndex);
            const float y = GetValue<float>::Value(state, stackIndex);
            const float x = GetValue<float>::Value(state, stackIndex);
            return glm::vec4(x, y, z, w);
        } else if constexpr (std::is_same_v<ArgType, glm::ivec4>) {
            // direction is reversed because stackIndex is decreasing
            const int32_t w = GetValue<int32_t>::Value(state, stackIndex);
            const int32_t z = GetValue<int32_t>::Value(state, stackIndex);
            const int32_t y = GetValue<int32_t>::Value(state, stackIndex);
            const int32_t x = GetValue<int32_t>::Value(state, stackIndex);
            return glm::ivec4(x, y, z, w);
        } else if constexpr (std::is_same_v<ArgType, glm::vec3>) {
            // direction is reversed because stackIndex is decreasing
            const float z = GetValue<float>::Value(state, stackIndex);
            const float y = GetValue<float>::Value(state, stackIndex);
            const float x = GetValue<float>::Value(state, stackIndex);
            return glm::vec3(x, y, z);
        } else if constexpr (std::is_same_v<ArgType, glm::quat>) {
            // direction is reversed because stackIndex is decreasing
            const float w = GetValue<float>::Value(state, stackIndex);
            const float z = GetValue<float>::Value(state, stackIndex);
            const float y = GetValue<float>::Value(state, stackIndex);
            const float x = GetValue<float>::Value(state, stackIndex);
            return glm::quat(w, x, y, z);
        } else if constexpr (std::is_void_v<ArgType>) {
        }
    }
};

template<typename tuple_type, size_t argsCount>
struct GetArgsPack {
    using arg_type = typename std::tuple_element<argsCount - 1, tuple_type>::type;

    FORCEINLINE static void Collect(lua_State* state, tuple_type& params, int32_t& stackIndex)
    {
        std::get<argsCount - 1>(params) = GetValue<arg_type>::Value(state, stackIndex);
        GetArgsPack<tuple_type, argsCount - 1>::Collect(state, params, stackIndex);
    }
};

template<typename tuple_type>
struct GetArgsPack<tuple_type, 0> {
    FORCEINLINE static void Collect(lua_State* state, tuple_type& params, int32_t& stackIndex)
    {
    }
};

struct GetGlobalBase {
    FORCEINLINE static void GetGlobal(const LuaWrapper& instanceWrapper, const std::string& variableName)
    {
        lua_getglobal(instanceWrapper.GetState(), variableName.c_str());
    }
};

template<typename FunctorType, typename ArgsPack_t, typename ReturnValueType>
struct CallbackInvoker {
    FORCEINLINE static int Invoke(lua_State* state, void* ownerPtr, const uint64_t funcHash, const ArgsPack_t& packArgs)
    {
        LuaScriptExecutorBase* baseExecutorInstance = reinterpret_cast<LuaScriptExecutorBase*>(ownerPtr);
        const auto functor_any = baseExecutorInstance->GetFunctorAny(funcHash);
        const auto& functor = std::any_cast<FunctorType>(functor_any);
        const auto retValue = functor(packArgs);
        ValuePusher<ReturnValueType>::Push(state, retValue);
        return 1;
    }
};

template<typename FunctorType, typename ArgsPack_t>
struct CallbackInvoker<FunctorType, ArgsPack_t, void> {
    FORCEINLINE static int Invoke(lua_State* state, void* ownerPtr, const uint64_t funcHash, const ArgsPack_t& packArgs)
    {
        LuaScriptExecutorBase* baseExecutorInstance = reinterpret_cast<LuaScriptExecutorBase*>(ownerPtr);
        const auto functor_any = baseExecutorInstance->GetFunctorAny(funcHash);
        const auto& functor = std::any_cast<FunctorType>(functor_any);
        functor(packArgs);
        return 0;
    }
};

template<typename ArgType>
FORCEINLINE static constexpr int32_t GetArgsCountForType()
{
    if constexpr (std::is_same_v<ArgType, glm::vec3> || std::is_same_v<ArgType, glm::ivec3>) {
        return 3;
    } else if constexpr (
        std::is_same_v<ArgType, glm::vec4> || std::is_same_v<ArgType, glm::ivec4> || std::is_same_v<ArgType, glm::quat>) {
        return 4;
    } else {
        return 1;
    }
}

template<typename tuple_t, int32_t currentIndex>
struct ArgsCounter {
    static constexpr int32_t value = GetArgsCountForType<typename std::tuple_element<currentIndex, tuple_t>::type>()
        + ArgsCounter<tuple_t, currentIndex - 1>::value;
};

template<typename tuple_t>
struct ArgsCounter<tuple_t, -1> {
    static constexpr int32_t value = 0;
};
/*------------ Inner Core  --------------*/
} // namespace LuaInnerCore

struct GetLuaGlobalVariable {
    template<typename VariableType>
    FORCEINLINE static VariableType
    Value(const LuaWrapper& instanceWrapper, const std::string& variableName, const int32_t stackIndex)
    {
        LuaInnerCore::GetGlobalBase::GetGlobal(instanceWrapper, variableName);
        int32_t localStackIndex = stackIndex;
        return LuaInnerCore::GetValue<VariableType>::Value(instanceWrapper, localStackIndex);
    }
};

template<typename FunctionType>
struct LuaFunctionInvoker;

template<typename RetType, typename... Args>
struct LuaFunctionInvoker<RetType(Args...)> {

    template<typename... TArgs>
    FORCEINLINE static RetType Invoke(const LuaWrapper& instanceWrapper, const std::string& functionName, TArgs&&... args)
    {
        lua_getglobal(instanceWrapper.GetState(), functionName.c_str());
        assert(lua_isfunction(instanceWrapper.GetState(), -1));

        LuaInnerCore::ChainValuesPusher<TArgs...>::Push(instanceWrapper, std::forward<TArgs>(args)...);

        static constexpr size_t argsCount = sizeof...(args);
        HasLuaError(instanceWrapper, lua_pcall(instanceWrapper.GetState(), argsCount, 1, /*error handling in lua*/ 0));

#ifdef DEBUG
        const auto& errorMsg = instanceWrapper.GetErrorMessageAt(-1);
        if (errorMsg.size() > 1) {
            std::cout << "ERROR: Lua script execution failed:" << errorMsg << std::endl;
            assert(false);
        }
#endif

        int32_t stackIndex = -1;
        return LuaInnerCore::GetValue<typename std::decay<RetType>::type>::Value(instanceWrapper, stackIndex);
    }
};

template<typename... Args>
struct LuaFunctionInvoker<void(Args...)> {

    template<typename... TArgs>
    FORCEINLINE static void Invoke(const LuaWrapper& instanceWrapper, const std::string& functionName, TArgs&&... args)
    {
        lua_getglobal(instanceWrapper.GetState(), functionName.c_str());
        assert(lua_isfunction(instanceWrapper.GetState(), -1));

        LuaInnerCore::ChainValuesPusher<TArgs...>::Push(instanceWrapper, std::forward<TArgs>(args)...);

        static constexpr size_t argsCount = sizeof...(args);
        HasLuaError(instanceWrapper, lua_pcall(instanceWrapper.GetState(), argsCount, 0, /*error handling in lua*/ 0));

#ifdef DEBUG
        const auto& errorMsg = instanceWrapper.GetErrorMessageAt(-1);
        if (errorMsg.size() > 1) {
            std::cout << "ERROR: Lua script execution failed:" << errorMsg << std::endl;
            LogInfo("ERROR: Lua script execution failed:", errorMsg);
            assert(false);
        }
#endif
    }
};

template<uint64_t funcHash, typename FunctorType>
struct LuaCallbackBinder;

template<uint64_t funcHash, typename ReturnType, typename... ArgsType>
struct LuaCallbackBinder<funcHash, ReturnType(ArgsType...)> {
    using this_t = LuaCallbackBinder<funcHash, ReturnType(ArgsType...)>;
    using args_t = std::tuple<ArgsType...>;
    using return_t = ReturnType;

    static constexpr auto sFuncHash = funcHash;

    FORCEINLINE static void Bind(const LuaWrapper& luaStateWrapper, const std::string& functionName)
    {
        lua_register(luaStateWrapper.GetState(), functionName.c_str(), this_t::InnerInvokeCallback);
    }

private:
    FORCEINLINE static int InnerInvokeCallback(lua_State* state)
    {
        using namespace LuaInnerCore;
        assert(lua_gettop(state) != 0); // Check missing host data
        auto ownerPtr = lua_touserdata(state, 1);
        assert(ownerPtr);

        constexpr size_t argsCount = sizeof...(ArgsType);
        auto topStackIndex
            = ArgsCounter<args_t, argsCount - 1>::value + 1; // + 1 because of host data at index 1

        args_t parameterPack;
        GetArgsPack<args_t, argsCount>::Collect(state, parameterPack, topStackIndex);

        return CallbackInvoker<std::function<return_t(args_t)>, args_t, return_t>::Invoke(
            state, ownerPtr, sFuncHash, parameterPack);
    }
};
} // namespace EngineCore::Scripts