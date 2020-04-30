#pragma once

#include <future>
#include "ResourceMap.h"

namespace IO {

   class AsyncDataProxy
   {
   public:
      std::map<std::string, std::future<Resource>> DATA;
   };

}

