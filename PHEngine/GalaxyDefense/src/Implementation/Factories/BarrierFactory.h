#pragma once

#include "ISpaceObjectFactory.h"

namespace EngineCore
{
    class Scene;
}

namespace Game
{
    class BarrierActor;

    class BarrierFactory
        : public IBarrierFactory
    {
        static size_t s_barrierCounter;

    public:
        BarrierFactory() = default;

        virtual std::shared_ptr<BarrierActor>
        CreateBarrier(const int32_t pillarsMeshCount,
                      const std::shared_ptr<::EngineCore::Scene> &scene,
                      const glm::vec3 &translation,
                      const glm::vec3 &rotation,
                      const glm::vec3 &scale) override;
    };
}