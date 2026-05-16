#include "ResourceMap.h"

#include "AsyncDataProxy.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/CommonCore/ResourceUsageObserver.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/IoCore/AsyncLoaderCore/AsyncJob.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/IoCore/ResourceExtensionsInfo.h"
#include "Core/ResourceManagerCore/Pool/MeshPool.h"
#include "Core/ResourceManagerCore/Pool/SoundBufferPool.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"

#include <algorithm>
#include <functional>

#define GET_FUL_PATH_TO_FILE(fileName) (IO::FolderManager::GetInstance()->GetAbsolutePath(fileName))

using namespace Resources;
using namespace TinyLogger;

namespace IO {

ResourceMap::ResourceMap()
    : mTextureLoader()
    , mMeshLoader()
    , mAudioLoader()
    , ReadyToReadResources()
    , mAsyncDataProxy(std::make_unique<AsyncDataProxy>())
{
}

ResourceMap* ResourceMap::GetInstance()
{
    static ResourceMap instance;
    return &instance;
}

void ResourceMap::CleanUp()
{
    ResourceUsageObserver::GetInstance()->CollectResourceConsumptionInfo();
    const double beforeCleanUpMemoryMb = ResourceUsageObserver::GetInstance()->GetLastMemoryUsageMegabytes();
    const bool bResourceWasntLoaded
        = std::any_of(ReadyToReadResources.begin(), ReadyToReadResources.end(), [this](const auto& resourcePair) {
              return !mAsyncDataProxy->ResourcesMap.count(resourcePair.first);
          });

    ext_assert(!bResourceWasntLoaded, "ResourceMap::CleanUp: Some resources are still loading or havent been loaded yet!");

    for (auto& [resourceName, resource] : ReadyToReadResources) {
        delete resource;
    }

    for (auto& [streamName, audioStreamResouce] : AudioStreamResources) {
        delete audioStreamResouce;
    }

    ReadyToReadResources.clear();
    AudioStreamResources.clear();
    mAsyncDataProxy->ResourcesMap.clear();

    ResourceUsageObserver::GetInstance()->CollectResourceConsumptionInfo();
    LogInfo(
        "ResourceMap::CleanUp: before cleanup memory mb: ",
        beforeCleanUpMemoryMb,
        ", after cleanup memory mb: ",
        ResourceUsageObserver::GetInstance()->GetLastMemoryUsageMegabytes());
}

ResourceMap::~ResourceMap()
{
    CleanUp();
}

bool ResourceMap::TryGetResource(Resource*& outResource, const std::string& key)
{
    const bool bIsValidResource = ReadyToReadResources.count(key) > 0;
    const bool bIsValidAudioStreamResource = AudioStreamResources.count(key) > 0;

    if (bIsValidResource || bIsValidAudioStreamResource) {
        outResource = bIsValidResource ? ReadyToReadResources[key] : AudioStreamResources[key];
    }

    return bIsValidResource || bIsValidAudioStreamResource;
}

void ResourceMap::UnloadResource(const std::string& key)
{
    if (ReadyToReadResources.count(key) > 0) {
        delete ReadyToReadResources[key];
        ReadyToReadResources.erase(key);
    }
}

void ResourceMap::AllocateAsync(const std::string& key)
{
    if (mAsyncDataProxy->ResourcesMap.count(key) > 0 || ReadyToReadResources.count(key) > 0) {
        LogInfo("ResourceMap::AllocateAsync: WARN! ResourceMap::AllocateAsync invoked for existing key! key = ", key);
        return;
    }

    const eResourceType resType = ResourceExtensionsInfo::GetResourceTypeByFileExtension(key);
    const std::string& fileFullPath = GET_FUL_PATH_TO_FILE(key);

    switch (resType) {
    case eResourceType::TEXTURE: {
        AsyncJob<Resource*, const std::string&> job(
            std::bind(&TextureResourceLoader::LoadResource, &mTextureLoader, std::placeholders::_1));
        std::future<Resource*> futureResult = job.StartAsync(fileFullPath);
        mAsyncDataProxy->ResourcesMap[key] = std::move(futureResult);
        break;
    }
    case eResourceType::MESH: {
        AsyncJob<Resource*, const std::string&> job(
            std::bind(&MeshResourceLoader::LoadResource, &mMeshLoader, std::placeholders::_1));
        std::future<Resource*> futureResult = job.StartAsync(fileFullPath);
        mAsyncDataProxy->ResourcesMap[key] = std::move(futureResult);
        break;
    }
    case eResourceType::AUDIO: {
        AsyncJob<Resource*, const std::string&> job(
            std::bind(&AudioResourceLoader::LoadResource, &mAudioLoader, std::placeholders::_1));
        std::future<Resource*> futureResult = job.StartAsync(fileFullPath);
        mAsyncDataProxy->ResourcesMap[key] = std::move(futureResult);
    }
    default:
        break;
    }

    LogInfo("ResourceMap::AllocateAsync: resource type: ", (uint8_t)resType, ", file: ", fileFullPath);
}

void ResourceMap::AllocateSync(const std::string& key)
{
    if (mAsyncDataProxy->ResourcesMap.count(key) > 0 || ReadyToReadResources.count(key) > 0) {
        LogInfo("ResourceMap::AllocateAsync: WARN! ResourceMap::AllocateSync invoked for existing key! key = ", key);
        return;
    }

    const eResourceType resType = ResourceExtensionsInfo::GetResourceTypeByFileExtension(key);
    const std::string& fileFullPath = GET_FUL_PATH_TO_FILE(key);

    switch (resType) {
    case eResourceType::TEXTURE: {
        AsyncJob<Resource*, const std::string&> job(
            std::bind(&TextureResourceLoader::LoadResource, &mTextureLoader, std::placeholders::_1));
        std::future<Resource*> futureResult = job.StartDeferred(fileFullPath);
        ReadyToReadResources[key] = futureResult.get();
        break;
    }
    case eResourceType::MESH: {
        AsyncJob<Resource*, const std::string&> job(
            std::bind(&MeshResourceLoader::LoadResource, &mMeshLoader, std::placeholders::_1));
        std::future<Resource*> futureResult = job.StartDeferred(fileFullPath);
        ReadyToReadResources[key] = futureResult.get();
        break;
    }
    case eResourceType::AUDIO: {
        AsyncJob<Resource*, const std::string&> job(
            std::bind(&AudioResourceLoader::LoadResource, &mAudioLoader, std::placeholders::_1));
        std::future<Resource*> futureResult = job.StartAsync(fileFullPath);
        mAsyncDataProxy->ResourcesMap[key] = std::move(futureResult);
        break;
    }
    default:
        break;
    }
}

void ResourceMap::OpenAudioStream(const std::string& key)
{
    if (AudioStreamResources.count(key) > 0) {
        LogInfo("ResourceMap::AllocateAsync: WARN! ResourceMap::AllocateSync invoked for existing key! key = ", key);
        return;
    }

    const eResourceType resType = ResourceExtensionsInfo::GetResourceTypeByFileExtension(key);
    const std::string& fileFullPath = GET_FUL_PATH_TO_FILE(key);
    ext_assert(resType == eResourceType::AUDIO, "ResourceMap::OpenAudioStream: Resource is not audio type! key = " + key);
    AudioStreamResources[key] = static_cast<AudioStreamResource*>(mAudioLoader.GetStreamResource(fileFullPath));
}

void ResourceMap::WaitUntilResourcesLoad()
{
    try {
        for (auto& resource : mAsyncDataProxy->ResourcesMap) {
            if (resource.second.valid()) {
                ReadyToReadResources[resource.first] = resource.second.get();
            }
        }
    } catch (const std::exception& e) {
        throw e.what();
    }
}
} // namespace IO