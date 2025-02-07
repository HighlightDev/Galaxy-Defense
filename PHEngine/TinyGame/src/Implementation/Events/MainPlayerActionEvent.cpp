#include "MainPlayerActionEvent.h"

using namespace Event;

namespace Event {
template class TEvent<eEventThreadType::GAME_THREAD, SingleDataEventPolicy<eMainPlayerActionEnum>>;
}