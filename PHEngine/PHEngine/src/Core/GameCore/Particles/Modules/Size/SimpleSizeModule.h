#pragma once

#include "Core/GameCore/Particles/Modules/Size/ISizeModule.h"

namespace EngineCore {
class SimpleSizeModule : public ISizeModule {
    float mSizeBegin;

    float mSizeEnd;

public:
    SimpleSizeModule();

    void Update(Particle& particle, const float deltaTime) override;

    void SetSizeBegin(const float sizeBegin);

    void SetSizeEnd(const float sizeEnd);

    void OnEmitParticles() override;
};
} // namespace EngineCore