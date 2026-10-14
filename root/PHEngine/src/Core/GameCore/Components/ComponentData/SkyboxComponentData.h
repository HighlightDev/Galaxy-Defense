#pragma once

#include "Core/GameCore/Components/ComponentData/ComponentData.h"

#include <string>

namespace EngineCore {
struct SkyboxComponentData : public ComponentData {
    SkyboxComponentData(
        const std::string& gameObjectName,
        const glm::vec3& scale,
        const std::shared_ptr<Graphics::IMaterial>& material,
        const bool isEnabled = true,
        const bool isVisible = true)
        : ComponentData(gameObjectName)
        , m_scale(scale)
        , m_material(material)
        , mIsEnabled(isEnabled)
        , mIsVisible(isVisible)
    {
    }

    glm::vec3 m_scale;

    std::shared_ptr<Graphics::IMaterial> m_material;

    bool mIsEnabled;
    bool mIsVisible;
};

} // namespace EngineCore