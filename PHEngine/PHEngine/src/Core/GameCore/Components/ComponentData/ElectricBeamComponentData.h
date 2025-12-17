#pragma once

#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GraphicsCore/Material/IMaterial.h"

#include <glm/vec3.hpp>

namespace EngineCore {

struct ElectricBeamComponentData : public ComponentData {
    glm::vec3 StartPoint;
    glm::vec3 EndPoint;
    float BeamThickness;
    int BeamCount;
    float JitterAmount;
    float UpdateFrequency;
    std::shared_ptr<Graphics::IMaterial> m_material;

    ElectricBeamComponentData(
        const std::string& gameObjectName,
        const glm::vec3& startPoint,
        const glm::vec3& endPoint,
        float beamThickness,
        int beamCount,
        float jitterAmount,
        float updateFrequency,
        const std::shared_ptr<Graphics::IMaterial>& material)
        : ComponentData(gameObjectName)
        , StartPoint(startPoint)
        , EndPoint(endPoint)
        , BeamThickness(beamThickness)
        , BeamCount(beamCount)
        , JitterAmount(jitterAmount)
        , UpdateFrequency(updateFrequency)
        , m_material(material)
    {
    }
};

} // namespace EngineCore
