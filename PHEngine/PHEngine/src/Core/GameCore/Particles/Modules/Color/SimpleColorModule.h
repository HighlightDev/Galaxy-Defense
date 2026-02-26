#pragma once

#include "Core/GameCore/Particles/Modules/Color/IColorModule.h"

#include <glm/vec3.hpp>

namespace EngineCore {
class SimpleColorModule : public IColorModule {
    glm::vec3 mColorBegin;
    glm::vec3 mColorEnd;

public:
    SimpleColorModule();

    void Update(Particle& particle, const float deltaTimeSec) override;

    void SetColorBegin(const glm::vec3& colorBegin);

    void SetColorEnd(const glm::vec3& colorEnd);

    void OnEmitParticles() override;

    std::shared_ptr<IGpuParticleModuleProxy> GetGpuProxy() const override;
};
} // namespace EngineCore