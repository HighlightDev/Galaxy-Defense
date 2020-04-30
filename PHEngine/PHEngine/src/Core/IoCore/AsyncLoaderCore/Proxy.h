#pragma once

#include <future>
#include "ResourceMap.h"

namespace IO {

   class Proxy
   {
   public:
      std::map<std::string, std::future<Resource>> DATA;

   };

}

