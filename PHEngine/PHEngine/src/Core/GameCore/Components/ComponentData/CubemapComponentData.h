#pragma once

#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GraphicsCore/TextureAtlas/TextureAtlasSpaceRequest.h"

#include <glm/vec3.hpp>

#include <string>

using namespace Graphics;

namespace EngineCore {
struct CubemapComponentData : public ComponentData {
    CubemapComponentData(
        const std::string& gameObjectName,
        const glm::vec3& translation,
        const glm::vec3& rotation,
        const glm::vec3& scale,
        const TextureAtlasSpaceRequest& textureObtainer)

        : ComponentData(gameObjectName)
        , m_translation(translation)
        , m_eulerRotationDegrees(rotation)
        , m_scale(scale)
        , m_textureObtainer(textureObtainer)
    {
    }

    glm::vec3 m_translation;
    glm::vec3 m_eulerRotationDegrees;
    glm::vec3 m_scale;
    TextureAtlasSpaceRequest m_textureObtainer;
};

} // namespace EngineCore