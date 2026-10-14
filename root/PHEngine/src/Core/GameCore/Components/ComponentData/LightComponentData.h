#pragma once

#include "ComponentData.h"

#include <glm/vec3.hpp>

#include <memory>

namespace Graphics {
class ProjectedShadowInfo;
}

namespace EngineCore {

struct LightComponentData : public ComponentData {

    LightComponentData(
        const std::string& gameObjectName,
        const glm::vec3& ambient,
        const glm::vec3& diffuse,
        const glm::vec3& specular,
        const std::shared_ptr<::Graphics::ProjectedShadowInfo>& shadowInfo,
        const glm::vec3& translation,
        const glm::vec3& rotateEuelerAngles,
        const glm::vec3& scale,
        const bool isEnabled,
        const bool isVisible)
        : ComponentData(gameObjectName)
        , Ambient(ambient)
        , Diffuse(diffuse)
        , Specular(specular)
        , ShadowInfo(shadowInfo)
        , Translation(translation)
        , Rotation(rotateEuelerAngles)
        , Scale(scale)
        , mIsEnabled(isEnabled)
        , mIsVisible(isVisible)
    {
    }

    glm::vec3 Ambient;
    glm::vec3 Diffuse;
    glm::vec3 Specular;
    std::shared_ptr<::Graphics::ProjectedShadowInfo> ShadowInfo;
    glm::vec3 Translation;
    glm::vec3 Rotation;
    glm::vec3 Scale;
    bool mIsEnabled;
    bool mIsVisible;
};

} // namespace EngineCore