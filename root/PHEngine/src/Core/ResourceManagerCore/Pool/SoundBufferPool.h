#pragma once

#include "Core/AudioCore/SoundBuffer.h"
#include "Core/ResourceManagerCore/Policy/SoundBufferAllocationPolicy.h"
#include "Core/ResourceManagerCore/Pool/PoolBase.h"

#include <string>

using namespace EngineCore;

namespace Resources {
class SoundBufferPool : public PoolBase<SoundBuffer, std::string, SoundBufferAllocationPolicy> {
    static std::unique_ptr<SoundBufferPool> m_instance;

public:
    using poolType_t = PoolBase<SoundBuffer, std::string, SoundBufferAllocationPolicy>;

    std::string ToString() const override;

    static std::unique_ptr<SoundBufferPool>& GetInstance();

    static void ReloadInstance();
};

} // namespace Resources
