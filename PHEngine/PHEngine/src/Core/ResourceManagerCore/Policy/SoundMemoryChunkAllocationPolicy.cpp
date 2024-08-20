#include "SoundMemoryChunkAllocationPolicy.h"
#include "Core/IoCore/AudioLoaderCore/AudioResourceInfo.h"
#include "Core/IoCore/AsyncLoaderCore/ResourceMap.h"
#include "Core/IoCore/RawResource.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/UtilityCore/PlatformDependentFunctions.h"

#include <AL/al.h>
#include <AL/alc.h>

using namespace IO;
using namespace IO::Audio;

namespace Resources
{
    std::shared_ptr<SoundMemoryChunk> SoundMemoryChunkAllocationPolicy::AllocateMemory(const std::string &arg)
    {
        Resource *outResource;
        const bool bResourceValid = ResourceMap::GetInstance()->TryGetResource(outResource, arg);
        assert(bResourceValid);

        const AudioResource *audioResource = static_cast<AudioResource *>(outResource);
        assert(audioResource);

        const size_t dataSize = audioResource->AudioInfo.mNumBytes;
        void *localData = malloc(dataSize);
        memcpy(localData, audioResource->DATA, dataSize);

        return std::make_shared<SoundMemoryChunk>(static_cast<short*>(localData), audioResource->AudioInfo);
    }

    void SoundMemoryChunkAllocationPolicy::DeallocateMemory(const std::shared_ptr<SoundMemoryChunk> &arg)
    {
        arg->CleanUp();
    }

}
