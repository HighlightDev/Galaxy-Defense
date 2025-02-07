#include "LogInterface.h"

namespace TinyLogger {
size_t Logger::index = 0;
typename Logger::Moment_t Logger::logStartTimestamp;
} // namespace TinyLogger