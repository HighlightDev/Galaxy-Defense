#include "ResourceLoader.h"

#include "Core/GameCore/LoggerExtension.h"
#include "Core/GraphicsCore/Texture/TexParams.h"
#include "Core/IoCore/AudioLoaderCore/SndFileLoader/SndFileLoader.h"
#include "Core/IoCore/AudioLoaderCore/StbFileLoader/StbImlementation.h"
#include "Core/IoCore/MeshLoaderCore/AssimpLoader/AssimpLoader.h"
#include "Core/IoCore/RawResource.h"
#include "Core/IoCore/TextureLoaderCore/StbLoader/StbLoader.h"
#include "Core/UtilityCore/PlatformDependentFunctions.h"
#include "ResourceMap.h"

#ifdef DEBUG
#include "Core/CommonCore/ResourceUsageObserver.h"
#endif

using namespace IO::Images;
using namespace IO::Images::Stb;
using namespace IO::Audio;
using namespace Graphics::Texture;
using namespace MeshLoader::Assimp;
using namespace EngineCore;

namespace IO {
/************************************************************************/
/* ResourceLoader                                                       */
/************************************************************************/
ResourceLoader::ResourceLoader()
{
}

/************************************************************************/
/* TextureResourceLoader                                                */
/************************************************************************/
TextureResourceLoader::TextureResourceLoader()
    : ResourceLoader()
{
}

Resource* TextureResourceLoader::LoadResource(const std::string& key)
{
    StbLoader textureResourceLoader;

    TextureResourceInfo texResourceInfo;
    uint8_t* data = textureResourceLoader.AllocateTextureMemoryFromFile(key, texResourceInfo);

    size_t size = texResourceInfo.Height * texResourceInfo.Width * texResourceInfo.PixelComponents;
    void* localData = malloc(size);
    memcpy(localData, data, size);
    textureResourceLoader.ReleaseTextureMemory(); // Release memory allocated for texture

    TextureResource* resource = new TextureResource();
    resource->mData = localData;
    resource->mTexInfo = texResourceInfo;

#ifdef DEBUG
    const auto& resObs = ResourceUsageObserver::GetInstance();
    resObs->CollectResourceConsumptionInfo();
    LogInfo(
        "TextureResourceLoader::LoadResource: ", key, ", memory mb after allocation: ", resObs->GetLastMemoryUsageMegabytes());
#else
    LogInfo("TextureResourceLoader::LoadResource: ", key);
#endif
    return resource;
}

/************************************************************************/
/* MeshResourceLoader                                                    */
/************************************************************************/

MeshResourceLoader::MeshResourceLoader()
    : ResourceLoader()
{
}

Resource* MeshResourceLoader::LoadResource(const std::string& key)
{
    const std::string& absolutePath = key;
    AssimpLoader loader(absolutePath);

    MeshResourceInfo* data = new MeshResourceInfo();

    data->meshAnimatedData = loader.GetAnimatedMeshData();
    data->meshAttributes = loader.GetMeshAttributes();

    MeshResource* resource = new MeshResource();
    resource->mData = data;

#ifdef DEBUG
    const auto& resObs = ResourceUsageObserver::GetInstance();
    resObs->CollectResourceConsumptionInfo();
    LogInfo("MeshResourceLoader::LoadResource: ", key, ", memory mb after allocation: ", resObs->GetLastMemoryUsageMegabytes());
#else
    LogInfo("MeshResourceLoader::LoadResource: ", key);
#endif

    return resource;
}

/************************************************************************/
/* AudioResourceLoader                                                  */
/************************************************************************/

AudioResourceLoader::AudioResourceLoader()
    : ResourceLoader()
{
}

Resource* AudioResourceLoader::LoadResource(const std::string& key)
{
    StbFileLoader stbLoader;
    AudioResourceInfo stbAudioResourceInfo;
    void* stbData = stbLoader.AllocateMemoryForAudioSource(key, stbAudioResourceInfo);
    const size_t dataSize = stbAudioResourceInfo.mNumBytes;
    void* localData = malloc(dataSize);
    memcpy(localData, stbData, stbAudioResourceInfo.mNumBytes);
    stbLoader.ReleaseAudioMemory();

    AudioResource* resource = new AudioResource();
    resource->mData = localData;
    resource->AudioInfo = stbAudioResourceInfo;

#ifdef DEBUG
    const auto& resObs = ResourceUsageObserver::GetInstance();
    resObs->CollectResourceConsumptionInfo();
    LogInfo("AudioResourceLoader::LoadResource: ", key, ", memory mb after allocation: ", resObs->GetLastMemoryUsageMegabytes());
#else
    LogInfo("AudioResourceLoader::LoadResource: ", key);
#endif
    return resource;
}

Resource* AudioResourceLoader::GetStreamResource(const std::string& key)
{
    StbFileLoader stbLoader;
    AudioResourceInfo stbAudioResourceInfo;
    AudioStreamResource* resource = new AudioStreamResource();
    resource->mStream = stbLoader.OpenStream(key, stbAudioResourceInfo);
    resource->mData = nullptr;
    resource->AudioInfo = stbAudioResourceInfo;
#ifdef DEBUG
    const auto& resObs = ResourceUsageObserver::GetInstance();
    resObs->CollectResourceConsumptionInfo();
    LogInfo(
        "AudioResourceLoader::GetStreamResource: ", key, ", memory mb after allocation: ", resObs->GetLastMemoryUsageMegabytes());
#else
    LogInfo("AudioResourceLoader::GetStreamResource: ", key);
#endif
    return resource;
}
} // namespace IO