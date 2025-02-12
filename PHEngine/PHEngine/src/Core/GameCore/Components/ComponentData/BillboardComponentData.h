#pragma once

#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GraphicsCore/Material/IMaterial.h"

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include <functional>
#include <string>

namespace EngineCore {
struct BillboardComponentData : public ComponentData {
    BillboardComponentData(
        const std::string& gameObjectName,
        const float billboardExtent,
        const glm::vec3& translation,
        const glm::vec3& scale,
        const std::shared_ptr<Graphics::IMaterial>& material,
        const std::function<glm::mat4(const glm::mat4&)> viewMatrixTransformer
        = std::function([](const glm::mat4& viewMatrix) { return viewMatrix; }),
        const std::function<glm::mat4(const glm::mat4&)> projectionMatrixTransformer
        = std::function([](const glm::mat4& projectionMatrix) { return projectionMatrix; }))
        : ComponentData(gameObjectName)
        , m_billboardExtent(billboardExtent)
        , m_translation(translation)
        , m_scale(scale)
        , m_material(material)
        , mViewMatrixTransformer(viewMatrixTransformer)
        , mProjectionMatrixTransformer(projectionMatrixTransformer)
    {
    }

    float m_billboardExtent;
    glm::vec3 m_translation;
    glm::vec3 m_scale;
    std::function<glm::mat4(const glm::mat4&)> mViewMatrixTransformer;
    std::function<glm::mat4(const glm::mat4&)> mProjectionMatrixTransformer;

    std::shared_ptr<Graphics::IMaterial> m_material;
};

} // namespace EngineCore