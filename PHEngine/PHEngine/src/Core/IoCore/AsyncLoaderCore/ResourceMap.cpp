#include "ResourceMap.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/IoCore/FolderManager.h"
#include "AsyncDataProxy.h"
#include "Core/IoCore/ResourceExtensionsInfo.h"
#include "Core/IoCore/AsyncLoaderCore/AsyncJob.h"
#include "Core/ResourceManagerCore/Pool/MeshPool.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"

#include <TinyLogger/LogInterface.h>

#include <functional>
#include <algorithm>

#define GET_REL_PATH_TO_FILE(fileName) (IO::FolderManager::GetInstance()->GetDirectoryRelativePathByFileName(fileName))
#define GET_FUL_PATH_TO_FILE(fileName) (IO::FolderManager::GetInstance()->GetPathToExeFile() + GET_REL_PATH_TO_FILE(fileName))

using namespace Resources;
using namespace TinyLogger;

namespace IO
{

   ResourceMap *ResourceMap::mInstance = nullptr;

   ResourceMap::ResourceMap()
       : ReadyToReadResources(), mAsyncDataProxy(new AsyncDataProxy())
   {
   }

   ResourceMap::~ResourceMap()
   {
      const bool bResourceWasntLoaded = std::any_of(ReadyToReadResources.begin(), ReadyToReadResources.end(), [this](const auto &resourcePair)
                                                    { return !mAsyncDataProxy->ResourcesMap.count(resourcePair.first); });

      assert(!bResourceWasntLoaded);

      for (auto &pair : ReadyToReadResources)
      {
         Resource *res = pair.second;
         res->Clear();
         delete res;
      }

      delete mAsyncDataProxy;
      mAsyncDataProxy = nullptr;
   }

   bool ResourceMap::TryGetResource(Resource *&outResource, const std::string &key)
   {
      const bool bValid = ReadyToReadResources.count(key) > 0;

      if (bValid)
      {
         outResource = ReadyToReadResources[key];
      }

      return bValid;
   }

   void ResourceMap::AllocateAsync(const std::string &key)
   {

      if (mAsyncDataProxy->ResourcesMap.count(key) > 0 || ReadyToReadResources.count(key) > 0)
      {
         Logger::Out("WARN! ResourceMap::AllocateAsync invoked for existing key! key = ", key);
         return;
      }

      const eResourceType resType = ResourceExtensionsInfo::GetResourceTypeByFileExtension(key);
      const std::string &fileFullPath = GET_FUL_PATH_TO_FILE(key);

      switch (resType)
      {
      case eResourceType::TEXTURE:
      {
         AsyncJob<Resource *, const std::string &> job(std::bind(&TextureResourceLoader::LoadResource, &textureLoader, std::placeholders::_1));
         std::future<Resource *> futureResult = job.StartAsync(fileFullPath);
         mAsyncDataProxy->ResourcesMap[key] = std::move(futureResult);
         break;
      }
      case eResourceType::MESH:
      {
         AsyncJob<Resource *, const std::string &> job(std::bind(&MeshResourceLoader::LoadResource, &meshLoader, std::placeholders::_1));
         std::future<Resource *> futureResult = job.StartAsync(fileFullPath);
         mAsyncDataProxy->ResourcesMap[key] = std::move(futureResult);
         break;
      }
      default:
         break;
      }
   }

   void ResourceMap::AllocateSync(const std::string &key)
   {
      if (mAsyncDataProxy->ResourcesMap.count(key) > 0 || ReadyToReadResources.count(key) > 0)
      {
         Logger::Out("WARN! ResourceMap::AllocateSync invoked for existing key! key = ", key);
         return;
      }

      const eResourceType resType = ResourceExtensionsInfo::GetResourceTypeByFileExtension(key);
      const std::string &fileFullPath = GET_FUL_PATH_TO_FILE(key);

      switch (resType)
      {
      case eResourceType::TEXTURE:
      {
         AsyncJob<Resource *, const std::string &> job(std::bind(&TextureResourceLoader::LoadResource, &textureLoader, std::placeholders::_1));
         std::future<Resource *> futureResult = job.StartDeferred(fileFullPath);
         ReadyToReadResources[key] = futureResult.get();
         break;
      }
      case eResourceType::MESH:
      {
         AsyncJob<Resource *, const std::string &> job(std::bind(&MeshResourceLoader::LoadResource, &meshLoader, std::placeholders::_1));
         std::future<Resource *> futureResult = job.StartDeferred(fileFullPath);
         ReadyToReadResources[key] = futureResult.get();
         break;
      }
      default:
         break;
      }
   }

   void ResourceMap::UploadLoadedResourcesToPool()
   {
      for (auto &resource : ReadyToReadResources)
      {
         switch (resource.second->ResourceType)
         {
         case eResourceType::MESH:
            MeshPool::GetInstance()->GetOrAllocateResource(resource.first);
            break;
         case eResourceType::TEXTURE:
            TexturePool::GetInstance()->GetOrAllocateResource(resource.first);
            break;
         default:
            assert(false); // undefined type
            break;
         }
      }
   }

   void ResourceMap::WaitUntilResourcesLoad()
   {
      try
      {
         for (auto &resource : mAsyncDataProxy->ResourcesMap)
         {
            ReadyToReadResources[resource.first] = resource.second.get();
         }
      }
      catch (const std::exception &e)
      {
         throw e.what();
      }
   }
}