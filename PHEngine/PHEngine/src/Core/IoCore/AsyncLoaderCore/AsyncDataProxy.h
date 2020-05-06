#pragma once

#include <future>

#include "Core/IoCore/RawResource.h"

namespace IO {

   class AsyncDataProxy
   {
   public:
      std::map<std::string, std::future<Resource*>> ResourcesMap;
   };

}

