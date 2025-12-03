#pragma once

#include "Core/GraphicsCore/Shadow/ProjectedShadowInfo.h"
#include "PointLightComponentData.h"

namespace EngineCore {

struct SpotlightComponentData : public PointLightComponentData {

    SpotlightComponentData(
        const std::string& gameObjectName,
        const glm::vec3& translation,
        const glm::vec3& rotation,
        const glm::vec3& attenuation,
        const float radianceRadius,
        const float cutoff,
        const glm::vec3& ambient,
        const glm::vec3& diffuse,
        const glm::vec3& specular,
        const std::shared_ptr<ProjectedShadowInfo>& shadowInfo,
        const bool isEnabled = true,
        const bool isVisible = false)
        : PointLightComponentData(
            gameObjectName,
            translation,
            attenuation,
            radianceRadius,
            ambient,
            diffuse,
            specular,
            shadowInfo,
            isEnabled,
            isVisible)
        , Cutoff(cutoff)
    {
        Rotation = rotation;
    }

    float Cutoff;

    ~SpotlightComponentData() override = default;
};

} // namespace EngineCore