#include "SoundStreamAllocationPolicy.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/IoCore/AsyncLoaderCore/ResourceMap.h"
#include "Core/IoCore/AudioLoaderCore/AudioResourceInfo.h"
#include "Core/IoCore/RawResource.h"
#include "Core/UtilityCore/PlatformDependentFunctions.h"

#include <AL/al.h>
#include <AL/alc.h>

using namespace IO;
using namespace IO::Audio;

namespace Resources {
std::shared_ptr<SoundStream> SoundStreamAllocationPolicy::AllocateMemory(const std::string& arg)
{
    LogInfo("SoundStreamAllocationPolicy::AllocateMemory: ", arg);
    Resource* outResource;
    const bool bResourceValid = ResourceMap::GetInstance()->TryGetResource(outResource, arg);
    ext_assert(bResourceValid, "SoundStreamAllocationPolicy::AllocateMemory: Resource not found: " + arg);

    const AudioStreamResource* audioResource = dynamic_cast<AudioStreamResource*>(outResource);
    ext_assert(audioResource, "SoundStreamAllocationPolicy::AllocateMemory: AudioStreamResource is null for: " + arg);

    return std::make_shared<SoundStream>(audioResource->mStream, audioResource->AudioInfo, arg);
}

void SoundStreamAllocationPolicy::DeallocateMemory(const std::shared_ptr<SoundStream>& arg)
{
    LogInfo("SoundStreamAllocationPolicy::DeallocateMemory: ", arg->GetStreamName());
    arg->CleanUp();
}

} // namespace Resources
