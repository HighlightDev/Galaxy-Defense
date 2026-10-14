#pragma once

#include "Core/GameCore/EngineObject.h"
#include "Core/GameCore/Event/WindowSizeChangedEvent.h"
#include "Core/GameCore/ITickable.h"
#include "Core/GameCore/Input/MouseEventEnums.h"
#include "Core/GraphicsCore/SceneProxy/CameraSceneProxy.h"
#include "Core/GraphicsCore/SceneViewInfo/CameraFrustum.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewProjectionInfo.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <atomic>
#include <optional>
#include <string>

using namespace Graphics;
using namespace Event;

namespace EngineCore {
class Scene;
class PlanarReflectionComponent;

enum eCameraType {
    UNINITIALIZED = 0,
    SECONDARY_FIRST_PERSON_CAMERA = 1,
    MAIN_FIRST_PERSON_CAMERA = (SECONDARY_FIRST_PERSON_CAMERA | (SECONDARY_FIRST_PERSON_CAMERA << 1)),
    SECONDARY_THIRD_PERSON_CAMERA = (1 << 2),
    MAIN_THIRD_PERSON_CAMERA = (SECONDARY_THIRD_PERSON_CAMERA | (SECONDARY_FIRST_PERSON_CAMERA << 3)),
};

class ACamera : public EngineObject,
                public ITickable,
                public std::enable_shared_from_this<ACamera>,
                public WindowSizeChangedGameThreadEvent {
    float m_rotateSensetivity;

    std::string mCameraName;

    std::shared_ptr<ViewProjectionInfo> mViewProjectionInfo;

    bool bTransformationDirty = false;

    std::atomic<bool> bIsCameraProxyReady{false};

protected:
    size_t mCameraProxyId{0};

    std::weak_ptr<Scene> mScene;

    std::shared_ptr<PlanarReflectionComponent> mPlanarReflectionComponent;

    ViewPortInfo mViewPort;

    glm::vec3 m_localSpaceRightVector;

    glm::vec3 m_localSpaceUpVector;

    glm::vec3 m_localSpaceForwardVector;

    glm::vec3 m_eyeSpaceRightVector;

    glm::vec3 m_eyeSpaceForwardVector;

    const glm::vec2 mPitchClampValue_min_max;

    float mYaw;

    float mPitch;

    eCameraType m_cameraType;

private:
    std::shared_ptr<EngineObjectProperty<glm::vec3>> mCameraPositionProperty;

public:
    ACamera(
        const std::string& cameraName,
        const eCameraType cameraType,
        std::shared_ptr<Scene> scene,
        const ViewPortInfo& viewPort,
        const std::shared_ptr<ViewProjectionInfo>& viewProjectionInfo,
        const float initPitchDeg,
        const float initYawDeg);

    virtual ~ACamera();

    virtual void Initialize();

    void SetCameraProxyId(const size_t proxyId);

    size_t GetCameraProxyId() const;

    void SetIsCameraProxyReady(const bool isReady);

    bool IsCameraProxyReady() const;

    void Tick(const float DeltaTime, const float playSpeed) override;

    void UnpausableTick(const float deltaTimeSec, const float playSpeed) override{};

    void ProcessEvent(
        const WindowSizeChangedGameThreadEvent* sender, const WindowSizeChangedGameThreadEvent::EventData_t& data) override;

    virtual void PostLevelInit();

    virtual glm::vec3 GetEyeVector() const = 0;

    virtual glm::vec3 GetTargetVector() const = 0;

    virtual glm::vec3 GetLocalSpaceUpVector() const = 0;

    virtual std::shared_ptr<CameraSceneProxy> CreateSceneProxy() const = 0;

    std::shared_ptr<PlanarReflectionComponent> GetPlanarReflectionComponent() const;

    std::string GetCameraName() const;

    eCameraType GetCameraType() const;

    virtual std::string GetCameraTypeName() const = 0;

    void SetPlanarReflectionComponent(std::shared_ptr<PlanarReflectionComponent> planarReflectionComponent);

    void SetLocalSpaceUpVector(glm::vec3& upVector);

    void SetLocalSpaceForwardVector(glm::vec3& forwardVector);

    void SetLocalSpaceRightVector(glm::vec3& rightVector);

    void SetCameraSensetivity(float rotateSensetivity);

    float GetCameraSensetivity() const;

    glm::vec3 GetLocalSpaceRightVector() const;

    glm::vec3 GetLocalSpaceForwardVector() const;

    glm::vec3 GetEyeSpaceForwardVector() const;

    glm::vec3 GetEyeSpaceRightVector() const;

    glm::mat4 GetViewMatrix() const;

    float GetRotationYaw() const;

    float GetRotationPitch() const;

    ViewPortInfo GetViewPort() const;

    const std::shared_ptr<ViewProjectionInfo>& GetViewProjectionInfo() const;

    void SetRotation(const int32_t deltaX, const int32_t deltaY);

    virtual void Zoom(eMouseScrollDirection zoomDirection, float zoomPower) = 0;

    glm::vec4 GetConvertedToClippedSpacePosition(const glm::vec4& worldPosition);

    glm::vec3 GetConvertedToNDCSpacePosition(const glm::vec4& worldPosition);

    glm::vec2 GetConvertedToTextureSpacePosition(const glm::vec4& worldPosition);

    std::optional<CameraFrustum> GetCameraFrustum() const;

    void OnCameraSceneProxyDataUpdated();

protected:
    virtual void UpdateRotationMatrix(int32_t deltaX, int32_t deltaY);

    virtual void OnTransformationUpdated();

    void SetTransformationDirty();

private:
    void UpdateCameraProxyData();
};

} // namespace EngineCore
