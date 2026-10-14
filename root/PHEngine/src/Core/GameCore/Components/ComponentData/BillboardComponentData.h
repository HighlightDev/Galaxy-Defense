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
        const bool isFlipped,
        const glm::vec3& scale,
        const std::shared_ptr<Graphics::IMaterial>& material,
        const std::function<glm::mat4(const glm::mat4&)> viewMatrixTransformer
        = std::function([](const glm::mat4& viewMatrix) { return viewMatrix; }),
        const std::function<glm::mat4(const glm::mat4&)> projectionMatrixTransformer
        = std::function([](const glm::mat4& projectionMatrix) { return projectionMatrix; }),
        const bool isEnabled = true,
        const bool isVisible = true)
        : ComponentData(gameObjectName, isEnabled)
        , m_billboardExtent(billboardExtent)
        , mApplyScreenAspectRatio(applyScreenAspectRatio)
        , m_translation(translation)
        , m_rotationRadians(rotationRadians)
        , mIsFlipped(isFlipped)
        , m_scale(scale)
        , m_material(material)
        , mViewMatrixTransformer(viewMatrixTransformer)
        , mProjectionMatrixTransformer(projectionMatrixTransformer)
        , mIsVisible(isVisible)
    {
    }

    float m_billboardExtent;
    bool mApplyScreenAspectRatio;
    glm::vec3 m_translation;
    float m_rotationRadians;
    bool mIsFlipped;
    glm::vec3 m_scale;
    std::function<glm::mat4(const glm::mat4&)> mViewMatrixTransformer;
    std::function<glm::mat4(const glm::mat4&)> mProjectionMatrixTransformer;
    bool mIsVisible;

    std::shared_ptr<Graphics::IMaterial> m_material;
};

} // namespace EngineCore