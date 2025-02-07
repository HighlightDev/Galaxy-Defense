#pragma once

#include "Core/IoCore/RawResource.h"

#include <future>

namespace IO {

class AsyncDataProxy {
public:
    std::map<std::string, std::future<Resource*>> ResourcesMap;
};

} // namespace IO
