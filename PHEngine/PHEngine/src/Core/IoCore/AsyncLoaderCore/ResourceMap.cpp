#include "ResourceMap.h"
#include "Core/CommonCore/Assertion.h"
#include "ResourceLoader.h"
#include <functional>

#include "AsyncDataProxy.h"

#include "Core/IoCore/ResourceExtensionsInfo.h"
#include "Core/IoCore/AsyncLoaderCore/AsyncJob.h"

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
            TextureResourceLoader loader;

            AsyncJob<Resource, const std::string&> job(std::bind(&TextureResourceLoader::LoadResource, &loader, std::placeholders::_1));
            std::future<Resource> futureResult = job.StartAsync(std::launch::async, key);
            mAsyncDataProxy->DATA[key] = std::move(futureResult);
            break;
         }
         case RESOURCE_TYPE::MESH:
         {
            MeshResourceLoader loader;
            std::future<Resource> futureResult = std::async(std::launch::async, std::bind(&MeshResourceLoader::LoadResource, &loader, key));
            mAsyncDataProxy->DATA[key] = std::move(futureResult);
            break;
         }
         default:
            break;
      }

      do 
      {
      } while (mAsyncDataProxy->DATA[key].valid());

      Resource result = mAsyncDataProxy->DATA[key].get();
   }

   void ResourceMap::AllocateSync(const std::string& key) {

   }

   void* ResourceMap::operator new(size_t size)
   {
      assert(false); // operator new definitely should not be called!

      return nullptr;
   }

}