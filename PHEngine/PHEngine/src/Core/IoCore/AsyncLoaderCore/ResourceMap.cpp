#include "ResourceMap.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/IoCore/FolderManager.h"
#include "AsyncDataProxy.h"
#include "Core/IoCore/ResourceExtensionsInfo.h"
#include "Core/IoCore/AsyncLoaderCore/AsyncJob.h"

#include <functional>

#define GET_REL_PATH_TO_FILE(fileName) (IO::FolderManager::GetInstance()->GetDirectoryRelativePathByFileName(fileName))
#define GET_FUL_PATH_TO_FILE(fileName) (IO::FolderManager::GetInstance()->GetPathToExeFile() + GET_REL_PATH_TO_FILE(fileName))

namespace IO {

   ResourceMap* ResourceMap::mInstance = nullptr;

   ResourceMap::ResourceMap()
      : ReadyToReadResources()
      , mAsyncDataProxy(new AsyncDataProxy())
   {
   }

   ResourceMap::~ResourceMap()
   {
      for (auto& pair : ReadyToReadResources)
      {
         Resource* res = pair.second;
         res->Clear();
         delete res;
      }

      delete mAsyncDataProxy;
      mAsyncDataProxy = nullptr;
   }

   bool ResourceMap::TryGetResource(Resource*& outResource, const std::string& key)
   {
      const bool bValid = ReadyToReadResources.count(key) > 0;

      if (bValid)
      {
         outResource = ReadyToReadResources[key];
      }

      return bValid;
   }

   void ResourceMap::AllocateAsync(const std::string& key) {

      const RESOURCE_TYPE resType = ResourceExtensionsInfo::GetResourceTypeByFileExtension(key);
      const std::string& fileFullPath = GET_FUL_PATH_TO_FILE(key);

      switch (resType)
      {
         case RESOURCE_TYPE::TEXTURE:
         {
            AsyncJob<Resource*, const std::string&> job(std::move(std::bind(&TextureResourceLoader::LoadResource, &textureLoader, std::placeholders::_1)));
            std::future<Resource*> futureResult = job.StartAsync(fileFullPath);
            mAsyncDataProxy->ResourcesMap[key] = std::move(futureResult);
            break;
         }
         case RESOURCE_TYPE::MESH:
         {
            AsyncJob<Resource*, const std::string&> job(std::move(std::bind(&MeshResourceLoader::LoadResource, &meshLoader, std::placeholders::_1)));
            std::future<Resource*> futureResult = job.StartAsync(fileFullPath);
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
      try
      {
         for (auto& resource : mAsyncDataProxy->ResourcesMap)
         {
            ReadyToReadResources[resource.first] = resource.second.get();
         }
      }
      catch (const std::exception& e)
      {
         throw e.what();
      }
   }
}