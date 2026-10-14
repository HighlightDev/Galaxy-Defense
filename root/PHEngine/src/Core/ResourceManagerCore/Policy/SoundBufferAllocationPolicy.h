#pragma once
#include "Core/AudioCore/SoundBuffer.h"

#include <memory>
#include <string>

using namespace EngineCore;

namespace Resources {
class SoundBufferAllocationPolicy {
public:
    static std::shared_ptr<SoundBuffer> AllocateMemory(const std::string& arg);

    static void DeallocateMemory(const std::shared_ptr<SoundBuffer>& arg);
};

} // namespace Resources
