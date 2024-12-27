#include "SoundStreamAllocationPolicy.h"
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
    std::shared_ptr<SoundStream> SoundStreamAllocationPolicy::AllocateMemory(const std::string &arg)
    {
        Resource *outResource;
        const bool bResourceValid = ResourceMap::GetInstance()->TryGetResource(outResource, arg);
        assert(bResourceValid);

        const AudioStreamResource *audioResource = dynamic_cast<AudioStreamResource *>(outResource);
        assert(audioResource);

        return std::make_shared<SoundStream>(audioResource->mStream, audioResource->AudioInfo);
    }

    void SoundStreamAllocationPolicy::DeallocateMemory(const std::shared_ptr<SoundStream> &arg)
    {
        arg->CleanUp();
    }

}
