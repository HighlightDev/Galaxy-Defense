#include "ResourceMap.h"
#include "Core/CommonCore/Assertion.h"
#include <functional>
#include "AsyncDataProxy.h"
#include "Core/IoCore/ResourceExtensionsInfo.h"
#include "Core/IoCore/AsyncLoaderCore/AsyncJob.h"

namespace IO {

   ResourceMap::ResourceMap() 
   : mAsyncDataProxy(new AsyncDataProxy())
   {

   }

   ResourceMap::~ResourceMap()
   {
      delete mAsyncDataProxy;
   }

   void ResourceMap::AllocateAsync(const std::string& key) {

      const RESOURCE_TYPE resType = ResourceExtensionsInfo::GetResourceTypeByFileExtension(key);

      switch (resType)
      {
         case RESOURCE_TYPE::TEXTURE:
         {  
            AsyncJob<Resource*, const std::string&> job(std::bind(&TextureResourceLoader::LoadResource, &textureLoader, std::placeholders::_1));
            std::future<Resource*> futureResult = job.StartAsync(std::launch::async, key);
            mAsyncDataProxy->ResourcesMap[key] = std::move(futureResult);
            break;
         }
         case RESOURCE_TYPE::MESH:
         {
            AsyncJob<Resource*, const std::string&> job(std::bind(&MeshResourceLoader::LoadResource, &meshLoader, std::placeholders::_1));
            std::future<Resource*> futureResult = job.StartAsync(std::launch::async, key);
            mAsyncDataProxy->ResourcesMap[key] = std::move(futureResult);
            break;
         }
         default:
            break;
      }
   }

   void ResourceMap::WaitUntilResourcesLoad()
   {
      while (true)
      {
         bool bResourcesLoaded = true;
         for (const auto& resource : mAsyncDataProxy->ResourcesMap)
         {
            if (!resource.second.valid())
            {
               bResourcesLoaded = false;
               break;
            }
         }

         if (bResourcesLoaded)
         {
            auto resource = mAsyncDataProxy->ResourcesMap.begin()->second.get();
            if (resource->ResourceType == RESOURCE_TYPE::TEXTURE)
            {
               auto texResource = static_cast<TextureResource*>(resource);
               int w = texResource->TexInfo.Width;
            }
            return;
         }
   

         std::this_thread::sleep_for(std::chrono::milliseconds(100));
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