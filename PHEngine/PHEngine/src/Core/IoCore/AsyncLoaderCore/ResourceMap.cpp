#include "ResourceMap.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/IoCore/FolderManager.h"
#include "AsyncDataProxy.h"
#include "Core/IoCore/ResourceExtensionsInfo.h"
#include "Core/IoCore/AsyncLoaderCore/AsyncJob.h"

#include <TinyLogger/LogInterface.h>

#include <functional>
#include <algorithm>

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
     const bool bResourceWasntLoaded = std::any_of(ReadyToReadResources.begin(), ReadyToReadResources.end(), [this](const auto& resourcePair) {
         return !mAsyncDataProxy->ResourcesMap.count(resourcePair.first); });

     assert(!bResourceWasntLoaded);

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

      if (mAsyncDataProxy->ResourcesMap.count(key) > 0 || ReadyToReadResources.count(key) > 0)
      {
         TinyLogger::LogProxy::LogMessages("WARN! ResourceMap::AllocateAsync invoked for existing key! key = ", std::string(key));
         return;
      }

      const RESOURCE_TYPE resType = ResourceExtensionsInfo::GetResourceTypeByFileExtension(key);
      const std::string& fileFullPath = GET_FUL_PATH_TO_FILE(key);

      switch (resType)
      {
         case RESOURCE_TYPE::TEXTURE:
         {
            AsyncJob<Resource*, const std::string&> job(std::bind(&TextureResourceLoader::LoadResource, &textureLoader, std::placeholders::_1));
            std::future<Resource*> futureResult = job.StartAsync(fileFullPath);
            mAsyncDataProxy->ResourcesMap[key] = std::move(futureResult);
            break;
         }
         case RESOURCE_TYPE::MESH:
         {
            AsyncJob<Resource*, const std::string&> job(std::bind(&MeshResourceLoader::LoadResource, &meshLoader, std::placeholders::_1));
            std::future<Resource*> futureResult = job.StartAsync(fileFullPath);
            mAsyncDataProxy->ResourcesMap[key] = std::move(futureResult);
            break;
         }
         default:
            break;
      }
   }

   void ResourceMap::AllocateSync(const std::string& key) 
   {
      if (mAsyncDataProxy->ResourcesMap.count(key) > 0 || ReadyToReadResources.count(key) > 0)
      {
         TinyLogger::LogProxy::LogMessages("WARN! ResourceMap::AllocateSync invoked for existing key! key = ", std::string(key));
         return;
      }

      const RESOURCE_TYPE resType = ResourceExtensionsInfo::GetResourceTypeByFileExtension(key);
      const std::string& fileFullPath = GET_FUL_PATH_TO_FILE(key);

      switch (resType)
      {
         case RESOURCE_TYPE::TEXTURE:
         {
            AsyncJob<Resource*, const std::string&> job(std::bind(&TextureResourceLoader::LoadResource, &textureLoader, std::placeholders::_1));
            std::future<Resource*> futureResult = job.StartDeferred(fileFullPath);
            ReadyToReadResources[key] = futureResult.get();
            break;
         }
         case RESOURCE_TYPE::MESH:
         {
            AsyncJob<Resource*, const std::string&> job(std::bind(&MeshResourceLoader::LoadResource, &meshLoader, std::placeholders::_1));
            std::future<Resource*> futureResult = job.StartDeferred(fileFullPath);
            ReadyToReadResources[key] = futureResult.get();
            break;
         }
         default:
            break;
      }
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