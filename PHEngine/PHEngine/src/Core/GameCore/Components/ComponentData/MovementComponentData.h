#pragma once

#include "Core/GameCore/ACamera.h"
#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GameCore/Components/Transform.h"

#include <glm/vec3.hpp>

#include <string>
#include <tuple>
#include <vector>

namespace EngineCore {
struct MovementComponentData : public ComponentData {
    MovementComponentData(const std::string& gameObjectName, const glm::vec3& launchDirection)
        : ComponentData(gameObjectName)
        , m_launchDirection(launchDirection)
    {
    }

    glm::vec3 m_launchDirection;
};

struct HumanoidMovementComponentData : public MovementComponentData {
    HumanoidMovementComponentData(
        const std::string& gameObjectName, const glm::vec3& launchDirection, const std::string& cameraName)
        : MovementComponentData(gameObjectName, launchDirection)
        , mCameraName(cameraName)
    {
    }

    std::string mCameraName;
};

struct PlatformTraverseComponentData : public ComponentData {
    PlatformTraverseComponentData(
        const std::string& gameObjectName, const std::vector<std::tuple<std::string, EulerAnglesTransform, float>>& routePoints)
        : ComponentData(gameObjectName)
        , mRoutePoints(routePoints)
    {
    }

    std::vector<std::tuple<std::string /*name*/, EulerAnglesTransform, float>> mRoutePoints;
};
} // namespace EngineCore