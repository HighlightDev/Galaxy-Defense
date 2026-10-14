#pragma once
#pragma once

#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"

#include <gl/glew.h>
#include <glm/vec3.hpp>

#include <memory>
#include <string>

namespace EngineCore {
class ACamera;

struct PlanarReflectionComponentData : public ComponentData {
    PlanarReflectionComponentData(
        const std::string& gameObjectName,
        const glm::vec3& translation,
        const glm::vec3& rotation,
        const glm::vec3& scale,
        const std::weak_ptr<ACamera>& ownerCameraWp,
        const ::Graphics::ViewPortInfo& fboViewPortInfo,
        const bool isEnabled = true)
        : ComponentData(gameObjectName, isEnabled)
        , m_translation(translation)
        , m_eulerRotationDegrees(rotation)
        , m_scale(scale)
        , m_ownerCamera(ownerCameraWp)
        , m_fboViewPortInfo(fboViewPortInfo)
    {
    }

    glm::vec3 m_translation;
    glm::vec3 m_eulerRotationDegrees;
    glm::vec3 m_scale;
    std::weak_ptr<ACamera> m_ownerCamera;
    ::Graphics::ViewPortInfo m_fboViewPortInfo;
};

} // namespace EngineCore