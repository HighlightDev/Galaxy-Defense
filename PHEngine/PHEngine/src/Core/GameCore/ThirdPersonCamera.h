#pragma once
#include "ACamera.h"
#include "Actor.h"
#include "Core/GameCore/Event/PlayerMovedEvent.h"

using namespace Event;

namespace EngineCore {
class Scene;
class InputComponent;

class ThirdPersonCamera : public ACamera, public PlayerMovedGameThreadEvent {
protected:
    float m_distanceFromTargetToCamera;

    std::shared_ptr<Actor> m_thirdPersonTarget;

    glm::vec3 m_actualTargetVector;

    glm::vec3 m_thirdPersonTargetOffset;

    float m_lerpTimeElapsed = 0.0f;

    float m_timeForInterpolation = 5e-1f;

    bool m_bThirdPersonTargetTransformationDirty = false;

    std::string mThirdPersonTargetGOName;

    bool bThirdPersonTargetDeferredDirty;

    std::unique_ptr<::EngineCore::InputComponent> mInputComponent;

    float m_maxDistanceFromTargetToCamera;

    float m_minDistanceFromTargetToCamera;

    static constexpr float sCameraMinDistance = 5.0f;
    static constexpr float sCameraMaxDistance = 135.0f;

    float m_targetDistanceFromTargetToCamera;

    float m_ZoomTime{0.0f};

    bool bZoomDirty{false};

    bool bZoomResetDirty{false};

public:
    ThirdPersonCamera(
        const std::string& cameraName,
        const eCameraType cameraType,
        const std::shared_ptr<Scene>& scene,
        const ViewPortInfo& viewPort,
        const std::shared_ptr<ViewProjectionInfo>& viewProjectionInfo,
        const float initPitchDeg,
        const float initYawDeg,
        const float camDistanceToThirdPersonTarget,
        const glm::vec3& thirdPersonTargetOffset = glm::vec3());

    ~ThirdPersonCamera() override;

    void Initialize() override;

    void Tick(const float DeltaTime) override;

    void
    ProcessEvent(const PlayerMovedGameThreadEvent* sender, const typename PlayerMovedGameThreadEvent::EventData_t& data) override;

    void UpdateRotationMatrix(int32_t deltaX, int32_t deltaY) override;

    void PostLevelInit() override;

    void SetMaxDistanceFromTargetToCamera(const float maxDistanceFromTargetToCamera);

    void SetMinDistanceFromTargetToCamera(const float minDistanceFromTargetToCamera);

    float GetMaxDistanceFromTargetToCamera() const;

    float GetMinDistanceFromTargetToCamera() const;

    float GetTimeForInterpolation() const;

    void SetTimeForInterpolation(float timeForInterpolation);

    virtual void ProcessZoom(const float deltaTimeSec);

    virtual void ProcessTargetFollow(const float deltaTimeSec);

    std::string GetCameraTypeName() const override;

    glm::vec3 GetLocalSpaceUpVector() const override;

    glm::vec3 GetEyeVector() const override;

    glm::vec3 GetTargetVector() const override;

    std::shared_ptr<CameraSceneProxy> CreateSceneProxy() const override;

    void Zoom(eMouseScrollDirection zoomDirection, float zoomPower) override;

    void SetDistanceFromTargetToCamera(float distanceFromTargetToCamera);

    float GetDistanceFromTargetToCamera() const;

    glm::vec3 GetThirdPersonTargetOffset() const;

    std::shared_ptr<Actor> GetThirdPersonTarget() const;

    void SetThirdPersonTargetDeferred(const std::string& targetEngineObjectName);

    void SetThirdPersonTarget(std::shared_ptr<Actor> thirdPersonTarget);

private:
    void ProcessDeferredThirdPersonTarget();
};
} // namespace EngineCore
