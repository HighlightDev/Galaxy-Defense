#pragma once

#include "LightRenderData.h"

namespace Graphics {
namespace Data {

struct DirectionalLightRenderData : public LightRenderData {
    glm::vec3 Direction;

    DirectionalLightRenderData(
        const glm::vec3& direction,
        const glm::vec3& ambient,
        const glm::vec3& diffuse,
        const glm::vec3& specular,
        const std::shared_ptr<ProjectedShadowInfo>& shadowInfo)
        : LightRenderData(ambient, diffuse, specular, shadowInfo)
        , Direction(direction)
    {
    }
};
} // namespace Data
} // namespace Graphics
