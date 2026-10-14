#pragma once

#include "Core/GameCore/Particles/Modules/ModuleGpuProxy/IGpuParticleModuleProxy.h"

#include <glm/vec3.hpp>

namespace EngineCore {
class SimpleVelocityModuleGpuProxy : public IGpuParticleModuleProxy {

    glm::vec3 mVelocityDirection;
    glm::vec3 mVelocityDeviation;

    glm::vec3 mCurrentSpawnVelocityDeviation;

    float mSpeed{1.0f};

public:
    SimpleVelocityModuleGpuProxy(const glm::vec3& velocityDirection, const glm::vec3& velocityDeviation, const float speed);

    std::string GetShaderSnippet() const override;

    uint64_t GetModuleTypeHash() const override;
};
} // namespace EngineCore