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
        const bool applyScreenAspectRatio,
        const glm::vec3& translation,
        const float rotationRadians,
        const glm::vec3& scale,
        const std::shared_ptr<Graphics::IMaterial>& material,
        const std::function<glm::mat4(const glm::mat4&)> viewMatrixTransformer
        = std::function([](const glm::mat4& viewMatrix) { return viewMatrix; }),
        const std::function<glm::mat4(const glm::mat4&)> projectionMatrixTransformer
        = std::function([](const glm::mat4& projectionMatrix) { return projectionMatrix; }))
        : ComponentData(gameObjectName)
        , m_billboardExtent(billboardExtent)
        , mApplyScreenAspectRatio(applyScreenAspectRatio)
        , m_translation(translation)
        , m_rotationRadians(rotationRadians)
        , m_scale(scale)
        , m_material(material)
        , mViewMatrixTransformer(viewMatrixTransformer)
        , mProjectionMatrixTransformer(projectionMatrixTransformer)
    {
    }

    float m_billboardExtent;
    bool mApplyScreenAspectRatio;
    glm::vec3 m_translation;
    float m_rotationRadians;
    glm::vec3 m_scale;
    std::function<glm::mat4(const glm::mat4&)> mViewMatrixTransformer;
    std::function<glm::mat4(const glm::mat4&)> mProjectionMatrixTransformer;

    std::shared_ptr<Graphics::IMaterial> m_material;
};

} // namespace EngineCore