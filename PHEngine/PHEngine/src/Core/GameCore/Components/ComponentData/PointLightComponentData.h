#pragma once

#include "LightComponentData.h"

namespace EngineCore {

struct PointLightComponentData : public LightComponentData {

    PointLightComponentData(
        const std::string& gameObjectName,
        glm::vec3 translation,
        glm::vec3 attenuation,
        const float radianceRadius,
        glm::vec3 ambient,
        glm::vec3 diffuse,
        glm::vec3 specular,
        const std::shared_ptr<ProjectedShadowInfo>& shadowInfo,
        const bool isEnabled = true,
        const bool isVisible = false)
        : LightComponentData(
              gameObjectName,
              ambient,
              diffuse,
              specular,
              shadowInfo,
              translation,
              glm::vec3(0),
              glm::vec3(1),
              isEnabled,
              isVisible)
        , Attenuation(attenuation)
        , RadianceRadius(radianceRadius)
    {
    }

    glm::vec3 Attenuation;
    float RadianceRadius;
};

} // namespace EngineCore