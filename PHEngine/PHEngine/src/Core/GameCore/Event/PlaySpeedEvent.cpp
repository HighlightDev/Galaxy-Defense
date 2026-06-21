#include "PlaySpeedEvent.h"

namespace Event {
template class TEvent<PlaySpeedGameThreadEvent, eEventThreadType::GAME_THREAD, SingleDataEventPolicy<float>>;
}
