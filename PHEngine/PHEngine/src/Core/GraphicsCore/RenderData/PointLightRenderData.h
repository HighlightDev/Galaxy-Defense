#pragma once

#include "LightRenderData.h"

namespace Graphics {
namespace Data {
struct PointLightRenderData : public LightRenderData {
    glm::vec3 Attenuation;
    float RadianceRadius;

    PointLightRenderData(
        const glm::vec3& attenuation,
        const float radianceRadius,
        const glm::vec3& ambient,
        const glm::vec3& diffuse,
        const glm::vec3& specular,
        const std::shared_ptr<ProjectedShadowInfo>& shadowInfo)
        : LightRenderData(ambient, diffuse, specular, shadowInfo)
        , Attenuation(attenuation)
        , RadianceRadius(radianceRadius)
    {
    }

    ~PointLightRenderData()
    {
    }
};
} // namespace Data
} // namespace Graphics
