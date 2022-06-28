#include "SoundBufferAllocationPolicy.h"
#include "Core/IoCore/AudioLoaderCore/AudioResourceInfo.h"
#include "Core/IoCore/AsyncLoaderCore/ResourceMap.h"
#include "Core/IoCore/RawResource.h"

#include "Core/CommonCore/Assertion.h"

#include "Core/UtilityCore/PlatformDependentFunctions.h"

using namespace IO;

namespace Resources
{
    std::shared_ptr<SoundBuffer> SoundBufferAllocationPolicy::AllocateMemory(const std::string &arg)
    {
        std::shared_ptr<SoundBuffer> resultSound;

        Resource *outResource;
        const bool bResourceValid = ResourceMap::GetInstance()->TryGetResource(outResource, arg);

        assert(bResourceValid);

        return resultSound;
    }
    void SoundBufferAllocationPolicy::DeallocateMemory(const std::shared_ptr<SoundBuffer> &arg)
    {
        arg->CleanUp();
    }

}
