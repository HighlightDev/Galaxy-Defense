#include "MainPlayerActionEvent.h"

namespace Event
{
    template class TEvent<eEventThreadType::GAME_THREAD, SingleDataEventPolicy<eMainPlayerActionEnum>>;
}