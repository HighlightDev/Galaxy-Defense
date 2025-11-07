#pragma once

#include "Core/CommonCore/Timer.h"
#include "Core/GameCore/BoundingBox3D.h"
#include "Core/GameCore/ThirdPersonCamera.h"
#include "Core/GraphicsCore/SceneViewInfo/CameraFrustum.h"
#include "Implementation/Events/LevelAreaBBChangedEvent.h"

#include <glm/vec2.hpp>

#include <vector>

using namespace EngineCore;
using namespace Graphics;

namespace Game {
class GalaxySceneCamera : public ThirdPersonCamera, public Event::LevelAreaBBChangedGameThreadEvent {

    static constexpr uint32_t s_userIdleTimeLimit{2000}; // after 2000 ms return camera to start position of scene

    static constexpr float s_cameraMovementAccelerationTime{0.5f};

    BoundingBox3D mLevelBoundaries;

    CameraFrustum mCameraFrustum;

    std::shared_ptr<GameThreadTimer> mFallbackToStartPositionTimer;

    bool bFallbackToStartPositionFlag{false};

    static constexpr float s_screenThresholdOffset{10.0f};

    float m_cameraMovementTime{0.0f};

public:
    GalaxySceneCamera(
        const std::string& cameraName,
        const eCameraType cameraType,
        std::shared_ptr<Scene> scene,
        const ViewPortInfo& viewPort,
        const std::shared_ptr<ViewProjectionInfo>& viewProjectionInfo,
        const float initPitchDeg,
        const float initYawDeg,
        const float camDistanceToThirdPersonTarget,
        const glm::vec3& thirdPersonTargetOffset = glm::vec3());

    ~GalaxySceneCamera() override;

    void Tick(const float deltaTimeSec) override;

    void Initialize() override;

    void InitializeMaxDistanceToCamera(const BoundingBox3D& levelBoundaries, const float FoVRadians);

    void ProcessEvent(
        const Event::LevelAreaBBChangedGameThreadEvent* sender,
        const typename Event::LevelAreaBBChangedGameThreadEvent::EventData_t& data) override;

protected:
    void OnTransformationUpdated() override;

    glm::vec3 GetEyeVector() const override;

private:
    void OnFallbackToStartPositionTimerTimeout();
};
} // namespace Game