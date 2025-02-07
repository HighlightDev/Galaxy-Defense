#pragma once

#include <any>
#include <functional>
#include <type_traits>
#include <utility>

#include "LuaCore.inl"

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
