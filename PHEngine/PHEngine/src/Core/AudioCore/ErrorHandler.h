#pragma once

#include <AL/al.h>
#include <AL/alc.h>

#include <type_traits>
#include <utility>

namespace EngineCore {
void CheckAlErrors();

void CheckAlcErrors(ALCdevice* device);

template<typename AlFunctor, typename... Args>
auto alCall(AlFunctor alFunctor, Args&&... args) ->
    typename std::enable_if<std::is_same<void, decltype(alFunctor(args...))>::value, void>::type
{
    alFunctor(std::forward<Args>(args)...);
    CheckAlErrors();
}

template<typename AlFunctor, typename... Args>
auto alCall(AlFunctor alFunctor, Args&&... args) ->
    typename std::enable_if<!std::is_same<void, decltype(alFunctor(args...))>::value, decltype(alFunctor(args...))>::type
{
    const auto retValue = alFunctor(std::forward<Args>(args)...);
    CheckAlErrors();
    return retValue;
}
} // namespace EngineCore