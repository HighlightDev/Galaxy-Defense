#pragma once

#include "ACamera.h"

#include <memory>

namespace EngineCore {
class Scene;
class InputComponent;

class FirstPersonCamera : public ACamera {
private:
    float m_cameraMoveSpeed;

    glm::vec3 m_firstPersonCameraPosition;

    std::unique_ptr<InputComponent> mInputComponent;

public:
    FirstPersonCamera(
        const std::string& cameraName,
        const eCameraType cameraType,
        std::shared_ptr<Scene> scene,
        const ViewPortInfo& viewPort,
        const std::shared_ptr<ViewProjectionInfo>& viewProjectionInfo,
        const float initPitchDeg,
        const float initYawDeg,
        glm::vec3 camPos);

    ~FirstPersonCamera();

    void Tick(const float DeltaTime) override;

    std::shared_ptr<CameraSceneProxy> CreateSceneProxy() const override;

    glm::vec3 GetEyeVector() const override;

    glm::vec3 GetTargetVector() const override;

    glm::vec3 GetLocalSpaceUpVector() const override;

    std::string GetCameraTypeName() const override;

    void MoveCamera(int32_t direction);

    float GetCameraMoveSpeed() const;

    void Zoom(eMouseScrollDirection zoomDirection, float zoomPower) override;
};

} // namespace EngineCore
