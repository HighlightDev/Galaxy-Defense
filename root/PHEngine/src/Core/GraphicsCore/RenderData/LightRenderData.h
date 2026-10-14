#pragma once

#include <glm/vec3.hpp>

#include <memory>

namespace Graphics {
class ProjectedShadowInfo;
}

namespace Graphics {
namespace Data {
struct LightRenderData {
    glm::vec3 Ambient;
    glm::vec3 Diffuse;
    glm::vec3 Specular;
    std::shared_ptr<::Graphics::ProjectedShadowInfo> ShadowInfo;

    LightRenderData(
        const glm::vec3& ambient,
        const glm::vec3& diffuse,
        const glm::vec3& specular,
        const std::shared_ptr<::Graphics::ProjectedShadowInfo>& shadowInfo)
        : Ambient(ambient)
        , Diffuse(diffuse)
        , Specular(specular)
        , ShadowInfo(shadowInfo)
    {
    }

    ~LightRenderData()
    {
    }
};
} // namespace Data
} // namespace Graphics
