#pragma once

#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GraphicsCore/Material/IMaterial.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <memory>
#include <string>
#include <vector>

namespace EngineCore {

// Emitter data structures
struct ParticleEmitterData {
    std::string emitterType; // "explosion", etc.
    float radius = 1.0f;
    size_t thetaSlicesCount = 10;
};

// Module data structures
struct LifeTimeModuleData {
    std::string moduleType; // "simple"
    float lifeTime = 1.0f;
};

struct ColorModuleData {
    std::string moduleType; // "simple"
    glm::vec3 colorBegin = glm::vec3(1.0f);
    glm::vec3 colorEnd = glm::vec3(1.0f);
};

struct SizeModuleData {
    std::string moduleType; // "simple"
    float sizeBegin = 1.0f;
    float sizeEnd = 1.0f;
};

struct VelocityModuleData {
    std::string moduleType; // "simple", "explosionInitial", "orbit"
    glm::vec3 velocityDirection = glm::vec3(0.0f);
    glm::vec3 velocityDeviation = glm::vec3(0.0f);
    float speed = 1.0f;
    float orbitRadius = 1.0f; // for orbit module
    float orbitHeight = 1.0f; // for orbit module
    float orbitAngularSpeed = 1.0f; // for orbit module
};

struct ParticleSystemComponentData : public ComponentData {
    ParticleSystemComponentData(
        const std::string& gameObjectName,
        const std::shared_ptr<Graphics::IMaterial>& materialInstance,
        const glm::vec3& translation,
        const glm::vec3& scale,
        const size_t particlesCount,
        const bool isEndlessRespawn)
        : ComponentData(gameObjectName)
        , m_translation(translation)
        , m_scale(scale)
        , m_particlesCount(particlesCount)
        , m_material(materialInstance)
        , m_isEndlessRespawn(isEndlessRespawn)
    {
    }

    glm::vec3 m_translation;
    glm::vec3 m_scale;
    size_t m_particlesCount;
    std::shared_ptr<Graphics::IMaterial> m_material;
    const bool m_isEndlessRespawn;

    // Particle modules configuration
    std::shared_ptr<ParticleEmitterData> emitterData;
    std::shared_ptr<LifeTimeModuleData> lifeTimeData;
    std::shared_ptr<ColorModuleData> colorData;
    std::shared_ptr<SizeModuleData> sizeData;
    std::vector<std::shared_ptr<VelocityModuleData>> velocityModules;
};

} // namespace EngineCore