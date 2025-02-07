#include "SoundBufferAllocationPolicy.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/IoCore/AsyncLoaderCore/ResourceMap.h"
#include "Core/IoCore/AudioLoaderCore/AudioResourceInfo.h"
#include "Core/IoCore/RawResource.h"
#include "Core/UtilityCore/PlatformDependentFunctions.h"

#include <AL/al.h>
#include <AL/alc.h>

using namespace IO;

namespace Resources {
std::shared_ptr<SoundBuffer> SoundBufferAllocationPolicy::AllocateMemory(const std::string& arg)
{
    LogInfo("SoundBufferAllocationPolicy::AllocateMemory: ", arg);
    Resource* outResource;
    const bool bResourceValid = ResourceMap::GetInstance()->TryGetResource(outResource, arg);
    assert(bResourceValid);

    const AudioResource* audioResource = static_cast<AudioResource*>(outResource);
    assert(audioResource);

    const auto& buffer = std::make_shared<SoundBuffer>(audioResource->mData, audioResource->AudioInfo);
    return buffer;
}
void SoundBufferAllocationPolicy::DeallocateMemory(const std::shared_ptr<SoundBuffer>& arg)
{
    LogInfo("SoundBufferAllocationPolicy::DeallocateMemory: ", arg->GetBufferDesc());
    arg->CleanUp();
}

} // namespace Resources
