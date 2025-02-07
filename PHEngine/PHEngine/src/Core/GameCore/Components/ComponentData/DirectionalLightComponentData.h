#pragma once

#include "LightComponentData.h"

namespace EngineCore {

struct DirectionalLightComponentData : public LightComponentData {

    DirectionalLightComponentData(
        const std::string& gameObjectName,
        const glm::vec3& rotation,
        const glm::vec3& direction,
        const glm::vec3& ambient,
        const glm::vec3& diffuse,
        const glm::vec3& specular,
        const std::shared_ptr<ProjectedShadowInfo>& shadowInfo)
        : LightComponentData(gameObjectName, ambient, diffuse, specular, shadowInfo, glm::vec3(), rotation, glm::vec3(1))
        , Direction(direction)
    {
    }

    glm::vec3 Direction;
};

} // namespace EngineCore