#include "GalaxySceneCamera.h"

#include "Core/GameCore/LoggerExtension.h"
#include "Core/IoCore/DisplayDeviceDataProvider.h"
#include "Core/UtilityCore/EngineMath.h"

using namespace IO;

namespace Game {
GalaxySceneCamera::GalaxySceneCamera(
    const std::string& cameraName,
    const eCameraType cameraType,
    std::shared_ptr<Scene> scene,
    const ViewPortInfo& viewPort,
    const std::shared_ptr<ViewProjectionInfo>& viewProjectionInfo,
    const float initPitchDeg,
    const float initYawDeg,
    const float camDistanceToThirdPersonTarget,
    const glm::vec3& thirdPersonTargetOffset)
    : ThirdPersonCamera(
          cameraName,
          cameraType,
          scene,
          viewPort,
          viewProjectionInfo,
          initPitchDeg,
          initYawDeg,
          camDistanceToThirdPersonTarget,
          thirdPersonTargetOffset)
    , mFallbackToStartPositionTimer()
{
    mFallbackToStartPositionTimer.SetIsPausable(true);
    mFallbackToStartPositionTimer.SetIsRepeat(true);
    mFallbackToStartPositionTimer.SetIntervalMs(s_userIdleTimeLimit);
    mFallbackToStartPositionTimer.SetCallback(std::bind(&GalaxySceneCamera::OnFallbackToStartPositionTimerTimeout, this));
}

void GalaxySceneCamera::OnFallbackToStartPositionTimerTimeout()
{
    bFallbackToStartPositionFlag = true;
}

void GalaxySceneCamera::OnTransformationUpdated()
{
    mCameraFrustum.ConstructFromViewProjectionMatrix(GetViewMatrix(), GetViewProjectionInfo()->CreateProjectionMatrix());
}

void GalaxySceneCamera::Tick(const float deltaTime)
{
    ACamera::Tick(deltaTime);

    const auto& mouseBindings = mInputComponent->GetMouseBindings();
    bool isUserMouseMoveIdle = true;

    if (mouseBindings->IsMouseMoveEventDirty() || mCameraFrustum.CollidesWithBoundingBox(mLevelBoundaries)) {
        mouseBindings->FlushMouseMoveEvent();
        isUserMouseMoveIdle = false;
    }

    if (mouseBindings->GetMouseMoveEventReceivedAtLeastOnce()) {
        const auto displayDeviceProvider = DisplayDeviceDataProvider::GetInstance();
        const auto& mouseMoveEvent = mouseBindings->GetLastMouseCursorPosition();
        if ((mouseMoveEvent.x <= s_screenThresholdOffset
             || mouseMoveEvent.x >= (displayDeviceProvider->GetWindowWidth() - s_screenThresholdOffset))
            || (mouseMoveEvent.y <= s_screenThresholdOffset
                || mouseMoveEvent.y >= (displayDeviceProvider->GetWindowHeight() - s_screenThresholdOffset))) {
            isUserMouseMoveIdle = false;
            const glm::vec2& windowPos = glm::vec2(
                static_cast<float>(displayDeviceProvider->GetWindowPosX()),
                static_cast<float>(displayDeviceProvider->GetWindowPosY()));
            const auto& centerOfScreen = glm::vec2(
                                             static_cast<float>(displayDeviceProvider->GetWindowWidth() >> 1),
                                             static_cast<float>(displayDeviceProvider->GetWindowHeight() >> 1))
                + windowPos;
            const auto& mousePosition = glm::vec2(static_cast<float>(mouseMoveEvent.x), static_cast<float>(mouseMoveEvent.y));
            const glm::vec2 nCameraMovementDir = glm::normalize(centerOfScreen - mousePosition);

            const float s_movementPower = 100.0f * deltaTime;
            m_actualTargetVector += glm::vec3(nCameraMovementDir.x * s_movementPower, 0, nCameraMovementDir.y * s_movementPower);
            SetTransformationDirty();
            bFallbackToStartPositionFlag = false;
        }
    }

    if (!isUserMouseMoveIdle) {
        mFallbackToStartPositionTimer.RestartTimer();
    }

    if (mouseBindings->IsMouseScrollEventDirty()) {
        const auto mouseZoomDirection = mouseBindings->FlushMouseScrollEvent();
        Zoom(mouseZoomDirection, 5.0f);
    }

    if (bFallbackToStartPositionFlag) {
        const auto& finalTargetVector = glm::vec3(0.0f);
        const auto& directionVector = finalTargetVector - m_actualTargetVector;
        const float distance = glm::length(directionVector);
        const auto nDirVec = directionVector * (1.0f / distance);
        const float s_cameraMovementSpeedPerTick = 200.0f * deltaTime;
        const float actualSpeed = distance <= s_cameraMovementSpeedPerTick ? distance : s_cameraMovementSpeedPerTick;
        m_actualTargetVector = m_actualTargetVector + nDirVec * actualSpeed;
        SetTransformationDirty();
        if (distance <= s_cameraMovementSpeedPerTick) {
            bFallbackToStartPositionFlag = false;
        }
    }
}

void GalaxySceneCamera::SetLevelBoundaries(const BoundingBox3D& levelBoundaries)
{
    mLevelBoundaries = levelBoundaries;
}
} // namespace Game