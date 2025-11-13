#pragma once

#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GraphicsCore/Material/IMaterial.h"

#include <glm/vec3.hpp>

#include <string>

namespace EngineCore {
struct ParticleSystemComponentData : public ComponentData {
    ParticleSystemComponentData(
        const std::string& gameObjectName,
        const std::shared_ptr<Graphics::IMaterial>& materialInstance,
        const glm::vec3& translation,
        const glm::vec3& scale,
        const size_t particlesCount)
        : ComponentData(gameObjectName)
        , m_translation(translation)
        , m_scale(scale)
        , m_particlesCount(particlesCount)
        , m_material(materialInstance)
    {
    }

    glm::vec3 m_translation;
    glm::vec3 m_scale;
    size_t m_particlesCount;
    std::shared_ptr<Graphics::IMaterial> m_material;
};

} // namespace EngineCore