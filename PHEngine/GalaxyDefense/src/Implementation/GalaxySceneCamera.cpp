#include "GalaxySceneCamera.h"

#include "Core/GameCore/DataProviders/GeneralSystemSettingsDataProvider.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPerspectiveInfo.h"
#include "Core/UtilityCore/EngineMath.h"

using namespace EngineCore::DataProviders;
using namespace Graphics;

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
    , mFallbackToStartPositionTimer(std::make_shared<GameThreadTimer>())
{
    mFallbackToStartPositionTimer->Initialize();
    mFallbackToStartPositionTimer->SetIsPausable(true);
    mFallbackToStartPositionTimer->SetIsRepeat(true);
    mFallbackToStartPositionTimer->SetIntervalMs(s_userIdleTimeLimit);
    mFallbackToStartPositionTimer->SetCallback(std::bind(&GalaxySceneCamera::OnFallbackToStartPositionTimerTimeout, this));
}

void GalaxySceneCamera::Initialize()
{
    ThirdPersonCamera::Initialize();

    Event::LevelAreaBBChangedGameThreadEvent::GetInstance()->AddListener(
        std::dynamic_pointer_cast<Event::LevelAreaBBChangedGameThreadEvent>(shared_from_this()));
}

GalaxySceneCamera::~GalaxySceneCamera()
{
    Event::LevelAreaBBChangedGameThreadEvent::GetInstance()->RemoveListener(
        Event::LevelAreaBBChangedGameThreadEvent::GetInstanceId());
}

void GalaxySceneCamera::OnFallbackToStartPositionTimerTimeout()
{
    bFallbackToStartPositionFlag = true;
}

void GalaxySceneCamera::OnTransformationUpdated()
{
    mCameraFrustum.ConstructFromViewProjectionMatrix(GetViewMatrix(), GetViewProjectionInfo()->CreateProjectionMatrix());
}

glm::vec3 GalaxySceneCamera::GetEyeVector() const
{
    assert(m_thirdPersonTarget);
    const float allowedDistance = m_maxDistanceFromTargetToCamera - m_minDistanceFromTargetToCamera;
    const float currentZoomCoef = (m_maxDistanceFromTargetToCamera - m_distanceFromTargetToCamera) / allowedDistance;
    const auto& directionVec = -GetEyeSpaceForwardVector();
    const float max_offset_y = (directionVec * (allowedDistance + m_minDistanceFromTargetToCamera)).y;
    const float min_offset_y = (directionVec * m_minDistanceFromTargetToCamera).y;
    const float offset_y = min_offset_y + (max_offset_y * std::cos(EngineMath::PI_HALF * currentZoomCoef));
    glm::vec3 offset = directionVec * m_distanceFromTargetToCamera;
    offset.y = offset_y;

    return GetTargetVector() + offset;
}

void GalaxySceneCamera::Tick(const float deltaTimeSec)
{
    ACamera::Tick(deltaTimeSec);

    const auto& mouseBindings = mInputComponent->GetMouseBindings();
    bool isUserMouseMoveIdle = true;

    if (mouseBindings->IsMouseMoveEventDirty() || mCameraFrustum.CollidesWithBoundingBox(mLevelBoundaries)) {
        mouseBindings->FlushMouseMoveEvent();
        isUserMouseMoveIdle = false;
    }

    if (mouseBindings->GetMouseMoveEventReceivedAtLeastOnce()) {
        const auto displayDeviceProvider = GeneralSystemSettingsDataProvider::GetInstance();
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

            const float s_movementPower = 100.0f * deltaTimeSec;
            const auto& newTargetVector = m_actualTargetVector
                + glm::vec3(nCameraMovementDir.x * s_movementPower, 0, nCameraMovementDir.y * s_movementPower);

            const glm::vec2 levelBoundariesMin = glm::vec2(mLevelBoundaries.GetMin().x, mLevelBoundaries.GetMin().z);
            const glm::vec2 levelBoundariesMax = glm::vec2(mLevelBoundaries.GetMax().x, mLevelBoundaries.GetMax().z);
            const glm::vec2 cameraPosition = glm::vec2(newTargetVector.x, newTargetVector.z);
            const bool isInsideLevelBoundaries
                = EngineMath::TestPointInAABB(levelBoundariesMin, levelBoundariesMax, cameraPosition);
            if (isInsideLevelBoundaries) {
                m_actualTargetVector = EngineMath::LerpVec3(
                    glm::clamp(m_cameraMovementTime, 0.0f, s_cameraMovementAccelerationTime),
                    0.0f,
                    s_cameraMovementAccelerationTime,
                    m_actualTargetVector,
                    newTargetVector);
                SetTransformationDirty();
                bFallbackToStartPositionFlag = false;
                m_cameraMovementTime += deltaTimeSec;
            } else {
                m_cameraMovementTime = 0.0f;
            }
        } else {
            m_cameraMovementTime = 0.0f;
        }
    }

    if (!isUserMouseMoveIdle) {
        mFallbackToStartPositionTimer->RestartTimer();
    }

    if (mouseBindings->IsMouseScrollEventDirty()) {
        const auto mouseZoomDirection = mouseBindings->FlushMouseScrollEvent();
        Zoom(mouseZoomDirection, 8.0f);
    }

    ProcessZoom(deltaTimeSec);

    if (bFallbackToStartPositionFlag) {
        const auto& finalTargetVector = glm::vec3(0.0f);
        const auto& directionVector = finalTargetVector - m_actualTargetVector;
        const float distance = glm::length(directionVector);
        const auto nDirVec = directionVector * (1.0f / distance);
        const float s_cameraMovementSpeedPerTick = 200.0f * deltaTimeSec;
        const float actualSpeed = distance <= s_cameraMovementSpeedPerTick ? distance : s_cameraMovementSpeedPerTick;
        m_actualTargetVector = m_actualTargetVector + nDirVec * actualSpeed;
        SetTransformationDirty();
        if (distance <= s_cameraMovementSpeedPerTick) {
            bFallbackToStartPositionFlag = false;
        }
    }
}

void GalaxySceneCamera::InitializeMaxDistanceToCamera(const BoundingBox3D& levelBoundaries, const float FoVRadians)
{
    mLevelBoundaries = levelBoundaries;
    const float halfLevelWidth = std::abs(levelBoundaries.GetMax().x - levelBoundaries.GetMin().x) * 0.5f;
    float halfFovRadians = FoVRadians * 0.5f;
    halfFovRadians = std::clamp(halfFovRadians, 0.0f, EngineMath::PI_HALF - 0.01f); // avoid division by zero
    const float maxDistance = halfLevelWidth / std::tan(halfFovRadians);
    m_maxDistanceFromTargetToCamera = maxDistance;
}

void GalaxySceneCamera::ProcessEvent(
    const Event::LevelAreaBBChangedGameThreadEvent* sender,
    const typename Event::LevelAreaBBChangedGameThreadEvent::EventData_t& data)
{
    const auto& boundingBox2D = std::get<0>(data);
    const auto& bbOrigin = boundingBox2D.GetOrigin();
    const auto& bbHalfExtent = boundingBox2D.GetHalfExtent();
    const BoundingBox3D newBb
        = BoundingBox3D(glm::vec3(bbOrigin.x, 0.0f, bbOrigin.y), glm::vec3(bbHalfExtent.x, 25.0f, bbHalfExtent.y));
    const float FoVRadians = eProjectionType::PERSPECTIVE == GetViewProjectionInfo()->GetProjectionType()
        ? std::static_pointer_cast<ViewPerspectiveInfo>(GetViewProjectionInfo())->GetFoV()
        : 0.0f;
    InitializeMaxDistanceToCamera(newBb, FoVRadians);
}
} // namespace Game