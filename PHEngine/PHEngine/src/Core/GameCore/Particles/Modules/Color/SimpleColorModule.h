#pragma once

#include "Core/GameCore/Particles/Modules/Color/IColorModule.h"

#include <glm/vec4.hpp>

namespace EngineCore {
class SimpleColorModule : public IColorModule {
    glm::vec4 mColorBegin;
    glm::vec4 mColorEnd;

public:
    SimpleColorModule();

    void Update(Particle& particle, const float deltaTime) override;

    void SetColorBegin(const glm::vec4& colorBegin);

    void SetColorEnd(const glm::vec4& colorEnd);

    void OnEmitParticles() override;
};
} // namespace EngineCore