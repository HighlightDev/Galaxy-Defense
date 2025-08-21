#pragma once

#include <any>
#include <functional>
#include <type_traits>
#include <utility>

#include "LuaCore.inl"

/**
 * @file LuaBindingHelper.h
 * @brief Provides utilities for binding C++ functors and callbacks to Lua scripting engine.
 *
 * This file contains template structures to wrap C++ functors into std::any for generic storage,
 * and helpers for binding C++ functions as Lua callbacks using a hash-based lookup.
 *
 * @namespace EngineCore::Scripts
 * 
 * @tparam FunctorType The type of functor to wrap.
 * @struct WrapFunctorIntoAny
 * @brief Template structure to wrap a functor into std::any as a std::function.
 *
 * @tparam ReturnType The return type of the functor.
 * @tparam Args Parameter types of the functor.
 * @struct WrapFunctorIntoAny<ReturnType(Args...)>
 * @brief Specialization for wrapping a functor with specific signature into std::any.
 * @method GetWrappedFunctor Wraps the provided functor into std::any as std::function.
 *
 * @tparam functionHash Unique hash identifying the function.
 * @tparam FunctorType The type of functor to bind.
 * @struct LuaCallbackBindingHelper
 * @brief Template structure for binding C++ functors as Lua callbacks.
 *
 * @tparam functionHash Unique hash identifying the function.
 * @tparam ReturnType The return type of the callback.
 * @tparam ArgsType Parameter types of the callback.
 * @struct LuaCallbackBindingHelper<functionHash, ReturnType(ArgsType...)>
 * @brief Specialization for binding a functor with specific signature as a Lua callback.
 * @typedef tupledParamsPack_t Tuple type representing the parameter pack.
 * @method Bind Binds the functor to the Lua scripting engine and registers the callback.
 *   - @param luaInstance Reference to the Lua wrapper instance.
 *   - @param ownerPtr Pointer to the script executor owner.
 *   - @param f The functor to bind.
 *   - @param functionName Name of the Lua function to bind.
 */
namespace EngineCore {
namespace Scripts {
template<typename FunctorType>
struct WrapFunctorIntoAny;

template<typename ReturnType, typename... Args>
struct WrapFunctorIntoAny<ReturnType(Args...)> {
    template<typename FunctionType>
    static std::any GetWrappedFunctor(FunctionType&& f)
    {
        return std::make_any<std::function<ReturnType(Args...)>>(
            std::function<ReturnType(Args...)>(std::forward<FunctionType>(f)));
    }
};

template<uint64_t functionHash, typename FunctorType>
struct LuaCallbackBindingHelper;

template<uint64_t functionHash, typename ReturnType, typename... ArgsType>
struct LuaCallbackBindingHelper<functionHash, ReturnType(ArgsType...)> {
    using tupledParamsPack_t = std::tuple<ArgsType...>;

    template<typename FunctionType, typename ScriptExecutorType>
    static typename std::enable_if<std::is_base_of<LuaScriptExecutorBase, ScriptExecutorType>::value, void>::type
    Bind(const LuaWrapper& luaInstance, ScriptExecutorType* ownerPtr, FunctionType&& f, const std::string& functionName)
    {
        ownerPtr->AddFunctor(
            functionHash, WrapFunctorIntoAny<ReturnType(tupledParamsPack_t)>::GetWrappedFunctor(std::forward<FunctionType>(f)));
        LuaCallbackBinder<functionHash, ReturnType(ArgsType...)>::Bind(luaInstance, functionName);
    }
};
} // namespace Scripts
} // namespace EngineCore
