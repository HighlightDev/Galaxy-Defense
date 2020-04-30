#include "ResourceMap.h"
#include "Core/CommonCore/Assertion.h"
#include "AsyncLoader.h"
#include <functional>

#include "Proxy.h"

namespace IO {

   ResourceMap::ResourceMap() 
   : PROXY(new Proxy())
   {

   }

   void ResourceMap::AllocateAsync(const std::string& key) {

      TextureAsyncLoader loader;
      std::future<Resource> result = std::async(std::launch::async, std::bind(&TextureAsyncLoader::DoAsyncJob, &loader, key));

      PROXY->DATA[key] = std::move(result);
   }

   void ResourceMap::AllocateSync(const std::string& key) {

   }

   void* ResourceMap::operator new(size_t size)
   {
      assert(false); // operator new definitely should not be called!

      return nullptr;
   }

}