#include "LogInterface.h"

namespace TinyLogger
{
   size_t Logger::index = 0;
   typename Logger::Clock_t::time_point Logger::logStartTimestamp;
}