#include "ResourceMap.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/IoCore/FolderManager.h"
#include "AsyncDataProxy.h"
#include "Core/IoCore/ResourceExtensionsInfo.h"
#include "Core/IoCore/AsyncLoaderCore/AsyncJob.h"
#include "Core/ResourceManagerCore/Pool/MeshPool.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Core/ResourceManagerCore/Pool/SoundBufferPool.h"
#include "Core/GameCore/LoggerExtension.h"

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
       : mTextureLoader(),
         mMeshLoader(),
         mAudioLoader(),
         ReadyToReadResources(),
         mAsyncDataProxy(new AsyncDataProxy())
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
         LogInfo("ResourceMap::AllocateAsync => WARN! ResourceMap::AllocateAsync invoked for existing key! key = ", key);
         return;
      }

      const eResourceType resType = ResourceExtensionsInfo::GetResourceTypeByFileExtension(key);
      const std::string &fileFullPath = GET_FUL_PATH_TO_FILE(key);

      switch (resType)
      {
      case eResourceType::TEXTURE:
      {
         AsyncJob<Resource *, const std::string &> job(std::bind(&TextureResourceLoader::LoadResource, &mTextureLoader, std::placeholders::_1));
         std::future<Resource *> futureResult = job.StartAsync(fileFullPath);
         mAsyncDataProxy->ResourcesMap[key] = std::move(futureResult);
         break;
      }
      case eResourceType::MESH:
      {
         AsyncJob<Resource *, const std::string &> job(std::bind(&MeshResourceLoader::LoadResource, &mMeshLoader, std::placeholders::_1));
         std::future<Resource *> futureResult = job.StartAsync(fileFullPath);
         mAsyncDataProxy->ResourcesMap[key] = std::move(futureResult);
         break;
      }
      case eResourceType::AUDIO:
      {
         AsyncJob<Resource *, const std::string &> job(std::bind(&AudioResourceLoader::LoadResource, &mAudioLoader, std::placeholders::_1));
         std::future<Resource *> futureResult = job.StartAsync(fileFullPath);
         mAsyncDataProxy->ResourcesMap[key] = std::move(futureResult);
      }
      default:
         break;
      }

      LogInfo("ResourceMap::AllocateAsync => resource type: ", (uint8_t)resType, ", file: ", fileFullPath);
   }

   void ResourceMap::AllocateSync(const std::string &key)
   {
      if (mAsyncDataProxy->ResourcesMap.count(key) > 0 || ReadyToReadResources.count(key) > 0)
      {
         LogInfo("ResourceMap::AllocateAsync => WARN! ResourceMap::AllocateSync invoked for existing key! key = ", key);
         return;
      }

      const eResourceType resType = ResourceExtensionsInfo::GetResourceTypeByFileExtension(key);
      const std::string &fileFullPath = GET_FUL_PATH_TO_FILE(key);

      switch (resType)
      {
      case eResourceType::TEXTURE:
      {
         AsyncJob<Resource *, const std::string &> job(std::bind(&TextureResourceLoader::LoadResource, &mTextureLoader, std::placeholders::_1));
         std::future<Resource *> futureResult = job.StartDeferred(fileFullPath);
         ReadyToReadResources[key] = futureResult.get();
         break;
      }
      case eResourceType::MESH:
      {
         AsyncJob<Resource *, const std::string &> job(std::bind(&MeshResourceLoader::LoadResource, &mMeshLoader, std::placeholders::_1));
         std::future<Resource *> futureResult = job.StartDeferred(fileFullPath);
         ReadyToReadResources[key] = futureResult.get();
         break;
      }
      case eResourceType::AUDIO:
      {
         AsyncJob<Resource *, const std::string &> job(std::bind(&AudioResourceLoader::LoadResource, &mAudioLoader, std::placeholders::_1));
         std::future<Resource *> futureResult = job.StartAsync(fileFullPath);
         mAsyncDataProxy->ResourcesMap[key] = std::move(futureResult);
      }
      default:
         break;
      }
   }

   void ResourceMap::SaveToPool(const std::string &key)
   {
      const eResourceType resType = ResourceExtensionsInfo::GetResourceTypeByFileExtension(key);
      switch (resType)
      {
      case eResourceType::MESH:
         MeshPool::GetInstance()->GetOrAllocateResource(key);
         break;
      case eResourceType::TEXTURE:
         TexturePool::GetInstance()->GetOrAllocateResource(key);
         break;
      case eResourceType::AUDIO:
         SoundBufferPool::GetInstance()->GetOrAllocateResource(key);
      default:
         assert(false); // undefined type
         break;
      }
   }

   void ResourceMap::WaitUntilResourcesLoad()
   {
      try
      {
         for (auto &resource : mAsyncDataProxy->ResourcesMap)
         {
            if (resource.second.valid())
            {
               ReadyToReadResources[resource.first] = resource.second.get();
            }
         }
      }
      catch (const std::exception &e)
      {
         throw e.what();
      }
   }
}