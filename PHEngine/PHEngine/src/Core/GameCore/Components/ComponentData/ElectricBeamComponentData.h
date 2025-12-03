#pragma once

#include "Core/GameCore/Components/ComponentData/ComponentData.h"

#include <glm/vec3.hpp>

namespace EngineCore {

struct ElectricBeamComponentData : public ComponentData {
    glm::vec3 StartPoint;
    glm::vec3 EndPoint;
    glm::vec3 BeamColor;
    float BeamThickness;
    int BeamCount;
    float JitterAmount;
    float UpdateFrequency;
    bool IsActive;

    ElectricBeamComponentData(
        const std::string& gameObjectName,
        const glm::vec3& startPoint,
        const glm::vec3& endPoint,
        const glm::vec3& beamColor = glm::vec3(0.3f, 0.5f, 1.0f),
        float beamThickness = 2.0f,
        int beamCount = 3,
        float jitterAmount = 0.2f,
        float updateFrequency = 0.05f,
        bool isActive = true)
        : ComponentData(gameObjectName)
        , StartPoint(startPoint)
        , EndPoint(endPoint)
        , BeamColor(beamColor)
        , BeamThickness(beamThickness)
        , BeamCount(beamCount)
        , JitterAmount(jitterAmount)
        , UpdateFrequency(updateFrequency)
        , IsActive(isActive)
    {
    }
};

} // namespace EngineCore
