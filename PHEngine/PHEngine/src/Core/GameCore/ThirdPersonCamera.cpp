#include "ThirdPersonCamera.h"

#include "Core/GameCore/Components/InputComponent.h"
#include "Core/GameCore/Input/KeyboardBindings.h"
#include "Core/GameCore/Input/MouseBindings.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/Serialize/SerializeHelper.h"
#include "Core/GraphicsCore/SceneProxy/MainCameraSceneProxy.h"
#include "Core/UtilityCore/EngineMath.h"

#include <algorithm>

#undef max
#undef min

namespace EngineCore {

ThirdPersonCamera::ThirdPersonCamera(
    const std::string& cameraName,
    const eCameraType cameraType,
    const std::shared_ptr<Scene>& scene,
    const ViewPortInfo& viewPort,
    const std::shared_ptr<ViewProjectionInfo>& viewProjectionInfo,
    const float initPitchDeg,
    const float initYawDeg,
    const float camDistanceToThirdPersonTarget,
    const glm::vec3& thirdPersonTargetOffset)
    : ACamera(cameraName, cameraType, scene, viewPort, viewProjectionInfo, initPitchDeg, initYawDeg)
    , PlayerMovedGameThreadEvent()
    , mThirdPersonTargetGOName("")
    , bThirdPersonTargetDeferredDirty(false)
    , m_thirdPersonTargetOffset(thirdPersonTargetOffset)
    , mInputComponent(std::make_unique<InputComponent>(std::make_shared<ComponentData>(cameraName + "_InputComponent")))
{
    SetMaxDistanceFromTargetToCamera(sCameraMaxDistance);
    SetMinDistanceFromTargetToCamera(sCameraMinDistance);
    SetDistanceFromTargetToCamera(camDistanceToThirdPersonTarget);
    m_targetDistanceFromTargetToCamera = camDistanceToThirdPersonTarget;
}

ThirdPersonCamera::~ThirdPersonCamera()
{
    PlayerMovedGameThreadEvent::GetInstance()->RemoveListener(PlayerMovedGameThreadEvent::GetInstanceId());
}

void ThirdPersonCamera::Initialize()
{
    ACamera::Initialize();

    PlayerMovedGameThreadEvent::GetInstance()->AddListener(std::dynamic_pointer_cast<ThirdPersonCamera>(shared_from_this()));
}

void ThirdPersonCamera::ProcessEvent(
    const PlayerMovedGameThreadEvent* sender, const PlayerMovedGameThreadEvent::EventData_t& data)
{
    m_bThirdPersonTargetTransformationDirty = true;
    m_lerpTimeElapsed = 0.0f;
}

void ThirdPersonCamera::UpdateRotationMatrix(int32_t deltaX, int32_t deltaY)
{
    ACamera::UpdateRotationMatrix(deltaX, deltaY);
}

void ThirdPersonCamera::PostLevelInit()
{
    ACamera::PostLevelInit();

    ProcessDeferredThirdPersonTarget();
}

void ThirdPersonCamera::Tick(const float DeltaTime)
{
    ACamera::Tick(DeltaTime);

    const auto& mouseBindings = mInputComponent->GetMouseBindings();
    if (mouseBindings->IsMouseMoveEventDirty()) {
        const auto& mouseMoveEvent = mouseBindings->FlushMouseMoveEvent();
        SetRotation(mouseMoveEvent.z, mouseMoveEvent.w);
    }

    if (mouseBindings->IsMouseScrollEventDirty()) {
        const auto mouseZoomDirection = mouseBindings->FlushMouseScrollEvent();
        Zoom(mouseZoomDirection, 5.0f);
    }

    ProcessZoom(DeltaTime);
    ProcessTargetFollow(DeltaTime);
}

void ThirdPersonCamera::SetMaxDistanceFromTargetToCamera(const float maxDistanceFromTargetToCamera)
{
    if (!EngineMath::FloatsNearEqual(maxDistanceFromTargetToCamera, m_maxDistanceFromTargetToCamera)) {
        m_maxDistanceFromTargetToCamera = maxDistanceFromTargetToCamera;
        SetTransformationDirty();
    }
}

float ThirdPersonCamera::GetMaxDistanceFromTargetToCamera() const
{
    return m_maxDistanceFromTargetToCamera;
}

void ThirdPersonCamera::SetMinDistanceFromTargetToCamera(const float minDistanceFromTargetToCamera)
{
    if (!EngineMath::FloatsNearEqual(minDistanceFromTargetToCamera, m_minDistanceFromTargetToCamera)) {
        m_minDistanceFromTargetToCamera = minDistanceFromTargetToCamera;
        SetTransformationDirty();
    }
}

float ThirdPersonCamera::GetMinDistanceFromTargetToCamera() const
{
    return m_minDistanceFromTargetToCamera;
}

float ThirdPersonCamera::GetTimeForInterpolation() const
{
    return m_timeForInterpolation;
}

void ThirdPersonCamera::SetTimeForInterpolation(float timeForInterpolation)
{
    m_timeForInterpolation = timeForInterpolation;
}

glm::vec3 ThirdPersonCamera::GetLocalSpaceUpVector() const
{
    return m_localSpaceUpVector;
}

glm::vec3 ThirdPersonCamera::GetEyeVector() const
{
    assert(m_thirdPersonTarget);
    return GetTargetVector() - (GetEyeSpaceForwardVector() * m_distanceFromTargetToCamera);
}

glm::vec3 ThirdPersonCamera::GetTargetVector() const
{
    assert(m_thirdPersonTarget);
    return m_actualTargetVector + m_thirdPersonTargetOffset; // attach to "head"
}

void ThirdPersonCamera::SetDistanceFromTargetToCamera(float distanceFromTargetToCamera)
{
    m_distanceFromTargetToCamera
        = std::max(std::min(distanceFromTargetToCamera, m_maxDistanceFromTargetToCamera), m_minDistanceFromTargetToCamera);
    SetTransformationDirty();
}

void ThirdPersonCamera::ProcessZoom(const float deltaTime)
{
    if (bZoomDirty) {
        m_ZoomTime = std::min(m_ZoomTime + deltaTime, m_timeForInterpolation);
        const float diff = m_targetDistanceFromTargetToCamera - m_distanceFromTargetToCamera;

        const float lerpedDistance = EngineMath::LerpFloat(
            m_ZoomTime, 0.0f, m_timeForInterpolation, m_distanceFromTargetToCamera, m_targetDistanceFromTargetToCamera);
        SetDistanceFromTargetToCamera(lerpedDistance);

        if (EngineMath::FloatsNearEqual(m_ZoomTime, m_timeForInterpolation)) {
            m_ZoomTime = 0.0f;
            bZoomDirty = false;
        }
    }
}

void ThirdPersonCamera::ProcessTargetFollow(const float deltaTime)
{
    if (m_bThirdPersonTargetTransformationDirty) {
        const float clampedDeltaTime = std::max(deltaTime, 0.03f);
        m_lerpTimeElapsed = std::min(m_lerpTimeElapsed + clampedDeltaTime, m_timeForInterpolation);

        glm::vec3 finalTargetVector = m_thirdPersonTarget->GetRootComponent()->GetTranslation();
        m_actualTargetVector
            = EngineMath::LerpVec3(m_lerpTimeElapsed, 0.0f, m_timeForInterpolation, m_actualTargetVector, finalTargetVector);

        SetTransformationDirty();

        // If camera is at final position
        if (EngineMath::FloatsNearEqual(m_lerpTimeElapsed, m_timeForInterpolation)) {
            m_lerpTimeElapsed = 0.0f;
            m_bThirdPersonTargetTransformationDirty = false;
        }
    }
}

void ThirdPersonCamera::Zoom(eMouseScrollDirection zoomDirection, float zoomPower)
{
    switch (zoomDirection) {
    case eMouseScrollDirection::ZoomIn: {
        m_targetDistanceFromTargetToCamera = m_distanceFromTargetToCamera - zoomPower;
        bZoomDirty = true;
        break;
    }
    case eMouseScrollDirection::ZoomOut: {
        m_targetDistanceFromTargetToCamera = m_distanceFromTargetToCamera + zoomPower;
        bZoomDirty = true;
        break;
    }

    default:
        break;
    }
}

float ThirdPersonCamera::GetDistanceFromTargetToCamera() const
{
    return m_distanceFromTargetToCamera;
}

glm::vec3 ThirdPersonCamera::GetThirdPersonTargetOffset() const
{
    return m_thirdPersonTargetOffset;
}

std::shared_ptr<Actor> ThirdPersonCamera::GetThirdPersonTarget() const
{
    return m_thirdPersonTarget;
}

std::shared_ptr<CameraSceneProxy> ThirdPersonCamera::CreateSceneProxy() const
{
    if (eCameraType::MAIN_THIRD_PERSON_CAMERA == m_cameraType) {
        return std::make_shared<MainCameraSceneProxy>(this);
    } else {
        return std::make_shared<CameraSceneProxy>(this);
    }
}

std::string ThirdPersonCamera::GetCameraTypeName() const
{
    return eCameraType::MAIN_THIRD_PERSON_CAMERA == m_cameraType ? "MainThirdPersonCamera" : "ThirdPersonCamera";
}

void ThirdPersonCamera::CollectDataForSerialization(SerializeDataContainer& dataContainer)
{
    auto cameraData = SerializeHelper::GetSerializedDataCamera(this);
    dataContainer.Cameras.emplace_back(cameraData);
}

void ThirdPersonCamera::SetThirdPersonTargetDeferred(const std::string& targetEngineObjectName)
{
    bThirdPersonTargetDeferredDirty = true;
    mThirdPersonTargetGOName = targetEngineObjectName;
}

void ThirdPersonCamera::SetThirdPersonTarget(std::shared_ptr<Actor> thirdPersonTarget)
{
    assert(!bThirdPersonTargetDeferredDirty);

    m_thirdPersonTarget = thirdPersonTarget;
    m_actualTargetVector = thirdPersonTarget->GetRootComponent()->GetTranslation();

    ACamera::UpdateRotationMatrix(0, 0);
}

void ThirdPersonCamera::ProcessDeferredThirdPersonTarget()
{
    if (!m_thirdPersonTarget) {
        if (auto sceneSp = mScene.lock()) {
            assert(mThirdPersonTargetGOName != "");
            const auto& actor = sceneSp->GetActorByName(mThirdPersonTargetGOName);
            bThirdPersonTargetDeferredDirty = false;
            SetThirdPersonTarget(actor);
        }
    }
}
} // namespace EngineCore
