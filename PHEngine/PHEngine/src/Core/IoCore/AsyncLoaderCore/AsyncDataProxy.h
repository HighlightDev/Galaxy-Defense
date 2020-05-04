#pragma once

#include <future>

#include "Core/IoCore/RawResource.h"

namespace IO {

   class AsyncDataProxy
   {
   public:
      std::map<std::string, std::future<Resource*>> ResourcesMap;

      ~AsyncDataProxy()
      {
         for (auto& pair : ResourcesMap)
         {
            Resource* res = pair.second.get();
            res->Clear();
            delete res;
         }
      }
   };

}

