#pragma once

#include <any>
#include <functional>
#include <utility>

namespace EngineCore
{
    namespace Scripts
    {
        template <typename FunctorType>
        struct WrapFunctorIntoAny;

        template <typename ReturnType, typename... Args>
        struct WrapFunctorIntoAny<ReturnType(Args...)>
        {
            template <typename FunctionType>
            static std::any GetWrappedFunctor(FunctionType &&f)
            {
                return std::make_any<std::function<ReturnType(Args...)>>(std::function<ReturnType(Args...)>(std::forward<FunctionType>(f)));
            }
        };
    }
} // namespace EngineCore
