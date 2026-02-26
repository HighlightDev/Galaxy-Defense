#pragma once

#include "Core/GameCore/Particles/Modules/ModuleGpuProxy/IGpuParticleModuleProxy.h"

#include <glm/vec3.hpp>

namespace EngineCore {
class SimpleColorModuleGpuProxy : public IGpuParticleModuleProxy {

    glm::vec3 mColorBegin;
    glm::vec3 mColorEnd;

public:
    SimpleColorModuleGpuProxy(const glm::vec3& colorBegin, const glm::vec3& colorEnd);

    std::string GetShaderSnippet() const override;

    constexpr uint64_t GetModuleTypeHash() const override;
};
} // namespace EngineCore
