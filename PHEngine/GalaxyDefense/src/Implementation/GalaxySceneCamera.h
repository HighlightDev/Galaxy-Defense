#pragma once

#include "Core/GameCore/ThirdPersonCamera.h"
#include "Core/CommonCore/Timer.h"

#include <vector>

using namespace EngineCore;

namespace Game
{
    class GalaxySceneCamera : public ThirdPersonCamera
    {

        static constexpr uint32_t s_userIdleTimeLimit{3000}; // after 3000 ms return camera to start position of scene

        GameThreadTimer mFallbackToStartPositionTimer;

        bool bFallbackToStartPositionFlag{false};

    public:
        GalaxySceneCamera(const std::string &cameraName,
                          const eCameraType cameraType,
                          std::shared_ptr<Scene> scene,
                          const ViewPortInfo &viewPort,
                          const std::shared_ptr<ViewProjectionInfo> &viewProjectionInfo,
                          const float initPitchDeg,
                          const float initYawDeg,
                          const float camDistanceToThirdPersonTarget,
                          const glm::vec3 &thirdPersonTargetOffset = glm::vec3());

        void Tick(const float deltaTime) override;

    private:
        void OnFallbackToStartPositionTimerTimeout();
    };
}