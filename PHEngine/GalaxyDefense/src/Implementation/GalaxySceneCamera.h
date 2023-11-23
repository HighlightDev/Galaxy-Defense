#pragma once

#include "Core/GameCore/ThirdPersonCamera.h"

#include <vector>

namespace Game
{
    class GalaxySceneCamera : public ThirdPersonCamera
    {

    public:
        GalaxySceneCamera(const std::string &cameraName,
                          const eCameraType cameraType,
                          std::shared_ptr<Scene> scene,
                          const ViewPortInfo &viewPort,
                          const float initPitchDeg,
                          const float initYawDeg,
                          const float camDistanceToThirdPersonTarget,
                          const glm::vec3 &thirdPersonTargetOffset = glm::vec3());

        void Tick(const float deltaTime) override;
    };
}