#include "CameraTransformChangedEvent.h"

namespace Event {
template class TEvent<CameraTransformChangedGameThreadEvent, eEventThreadType::GAME_THREAD, SingleDataEventPolicy<ACamera*>>;
}