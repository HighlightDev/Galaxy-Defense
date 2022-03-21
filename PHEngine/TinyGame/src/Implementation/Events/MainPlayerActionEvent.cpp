#include "MainPlayerActionEvent.h"

using namespace Event;

namespace Event
{
    template class TEvent<SingleDataEventPolicy<eMainPlayerActionEnum>>;
}