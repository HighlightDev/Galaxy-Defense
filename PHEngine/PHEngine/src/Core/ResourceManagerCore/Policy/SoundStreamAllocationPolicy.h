#pragma once
#include "Core/AudioCore/SoundStream.h"

#include <memory>
#include <string>

using namespace EngineCore;

namespace Resources {
class SoundStreamAllocationPolicy {
public:
    static std::shared_ptr<SoundStream> AllocateMemory(const std::string& arg);

    static void DeallocateMemory(const std::shared_ptr<SoundStream>& arg);
};

} // namespace Resources
