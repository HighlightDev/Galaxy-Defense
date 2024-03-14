#pragma once

#include "Core/GameCore/ThirdPersonCamera.h"
#include "Core/CommonCore/Timer.h"
#include "Core/GameCore/BoundingBox3D.h"
#include "Core/GraphicsCore/SceneViewInfo/CameraFrustum.h"

#include <glm/vec2.hpp>
#include <vector>

using namespace EngineCore;
using namespace Graphics;

namespace Game
{
    class GalaxySceneCamera : public ThirdPersonCamera
    {

        static constexpr uint32_t s_userIdleTimeLimit{3000}; // after 3000 ms return camera to start position of scene

        BoundingBox3D mLevelBoundaries;

        CameraFrustum mCameraFrustum;

        GameThreadTimer mFallbackToStartPositionTimer;

        bool bFallbackToStartPositionFlag{false};

        static constexpr float s_screenThresholdOffset{10.0f};

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

        void SetLevelBoundaries(const BoundingBox3D &levelBoundaries);

    protected:
        void OnTransformationUpdated() override;

    private:
        void OnFallbackToStartPositionTimerTimeout();
    };
}