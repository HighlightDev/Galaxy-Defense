#pragma once

#include "Core/GameCore/FirstPersonCamera.h"

namespace Game
{
    class SpaceSceneCamera : public FirstPersonCamera
    {
    public:
        SpaceSceneCamera(const std::string &cameraName, const eCameraType cameraType,
                         std::shared_ptr<Scene> scene, const ViewPortInfo &viewPort, const float initPitchDeg,
                         const float initYawDeg, glm::vec3 camPos);

        virtual void Tick(const float deltaTime) override;
    };
}