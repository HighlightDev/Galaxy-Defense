#include "ResourceMap.h"
#include "Core/CommonCore/Assertion.h"
#include <functional>
#include "AsyncDataProxy.h"
#include "Core/IoCore/ResourceExtensionsInfo.h"
#include "Core/IoCore/AsyncLoaderCore/AsyncJob.h"

namespace IO {

   ResourceMap* ResourceMap::mInstance = nullptr;

   ResourceMap::ResourceMap() 
   : mAsyncDataProxy(new AsyncDataProxy())
   {
   }

   ResourceMap::~ResourceMap()
   {
      delete mAsyncDataProxy;
      mAsyncDataProxy = nullptr;
   }

   bool ResourceMap::TryGetResource(Resource*& outResource, const std::string& key)
   {
      std::future<Resource*>& asyncResource = mAsyncDataProxy->ResourcesMap[key];
      const bool bValid = asyncResource.valid();
      
      if (bValid)
      {
         outResource = asyncResource.get();
      }

      return bValid;
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

   void ResourceMap::AllocateSync(const std::string& key) {

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
            return;
         }

         std::this_thread::sleep_for(std::chrono::milliseconds(100));
      }
   }
}