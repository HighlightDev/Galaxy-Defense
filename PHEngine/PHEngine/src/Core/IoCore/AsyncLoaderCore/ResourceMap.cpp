#include "ResourceMap.h"
#include "Core/CommonCore/Assertion.h"
#include "AsyncLoader.h"
#include <functional>

#include "AsyncDataProxy.h"

#include "Core/IoCore/ResourceExtensionsInfo.h"

namespace IO {

   ResourceMap::ResourceMap() 
   : mAsyncDataProxy(new AsyncDataProxy())
   {

   }

   void ResourceMap::AllocateAsync(const std::string& key) {

      const RESOURCE_TYPE resType = ResourceExtensionsInfo::GetResourceTypeByFileExtension(key);

      switch (resType)
      {
         case RESOURCE_TYPE::TEXTURE:
         {  
            TextureAsyncLoader loader;
            std::future<Resource> result = std::async(std::launch::async, std::bind(&TextureAsyncLoader::DoAsyncJob, &loader, key));
            mAsyncDataProxy->DATA[key] = std::move(result);
            break;
         }
         case RESOURCE_TYPE::MESH:
         {
            MeshAsyncLoader loader;
            std::future<Resource> result = std::async(std::launch::async, std::bind(&MeshAsyncLoader::DoAsyncJob, &loader, key));
            mAsyncDataProxy->DATA[key] = std::move(result);
            break;
         }
         default:
            break;
      }
   }

   void ResourceMap::AllocateSync(const std::string& key) {

   }

   void* ResourceMap::operator new(size_t size)
   {
      assert(false); // operator new definitely should not be called!

      return nullptr;
   }

}