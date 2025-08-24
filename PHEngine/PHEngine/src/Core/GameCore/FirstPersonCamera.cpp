#include "FirstPersonCamera.h"

#include "Core/GameCore/Components/InputComponent.h"
#include "Core/GameCore/Input/KeyboardBindings.h"
#include "Core/GameCore/Input/MouseBindings.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/SceneProxy/MainCameraSceneProxy.h"

using namespace Graphics;

namespace EngineCore {

FirstPersonCamera::FirstPersonCamera(
    const std::string& cameraName,
    const eCameraType cameraType,
    std::shared_ptr<Scene> scene,
    const ViewPortInfo& viewPort,
    const std::shared_ptr<ViewProjectionInfo>& viewProjectionInfo,
    const float initPitchDeg,
    const float initYawDeg,
    glm::vec3 camPos)
    : ACamera(cameraName, cameraType, scene, viewPort, viewProjectionInfo, initPitchDeg, initYawDeg)
    , m_firstPersonCameraPosition(camPos)
    , m_cameraMoveSpeed(0.01f)
    , mInputComponent(std::make_unique<InputComponent>(std::make_shared<ComponentData>("FirstPersonCameraInputComponent")))
{
    ACamera::UpdateRotationMatrix(0, 0);
}

FirstPersonCamera::~FirstPersonCamera()
{
}

glm::vec3 FirstPersonCamera::GetEyeVector() const
{
    return m_firstPersonCameraPosition;
}

glm::vec3 FirstPersonCamera::GetTargetVector() const
{
    return m_firstPersonCameraPosition + (m_eyeSpaceForwardVector * m_cameraMoveSpeed);
}

glm::vec3 FirstPersonCamera::GetLocalSpaceUpVector() const
{
    return m_localSpaceUpVector;
}

void FirstPersonCamera::Tick(const float DeltaTime)
{
    const auto& mouseBindings = mInputComponent->GetMouseBindings();
    if (mouseBindings->IsMouseMoveEventDirty()) {
        const auto& mouseMoveEvent = mouseBindings->FlushMouseMoveEvent();
        SetRotation(mouseMoveEvent.z, mouseMoveEvent.w);
    }

    const auto& keyboardBindings = mInputComponent->GetKeyboardBindings();
    if (keyboardBindings->HasPressedKeys()) {
        int32_t moveDirection = -1;
        if (KeyState::PRESSED == keyboardBindings->GetKeyStateByActionType(eKeyActionType::ACTION_MOVE_FORWARD)) {
            moveDirection = 0;
        } else if (KeyState::PRESSED == keyboardBindings->GetKeyStateByActionType(eKeyActionType::ACTION_MOVE_BACK)) {
            moveDirection = 1;
        } else if (KeyState::PRESSED == keyboardBindings->GetKeyStateByActionType(eKeyActionType::ACTION_MOVE_LEFT)) {
            moveDirection = 3;
        } else if (KeyState::PRESSED == keyboardBindings->GetKeyStateByActionType(eKeyActionType::ACTION_MOVE_RIGHT)) {
            moveDirection = 2;
        }

        MoveCamera(moveDirection);
    }

    ACamera::Tick(DeltaTime);
}

std::shared_ptr<CameraSceneProxy> FirstPersonCamera::CreateSceneProxy() const
{
    if (eCameraType::MAIN_FIRST_PERSON_CAMERA == m_cameraType) {
        return std::make_shared<MainCameraSceneProxy>(this);
    } else {
        return std::make_shared<CameraSceneProxy>(this);
    }
}

std::string FirstPersonCamera::GetCameraTypeName() const
{
    return eCameraType::MAIN_THIRD_PERSON_CAMERA == m_cameraType ? "MainFirstPersonCamera" : "FirstPersonCamera";
}

void FirstPersonCamera::MoveCamera(int32_t direction)
{
    switch (direction) {
    case 0:
        m_firstPersonCameraPosition += GetEyeSpaceForwardVector() * m_cameraMoveSpeed;
        break;
    case 1:
        m_firstPersonCameraPosition -= GetEyeSpaceForwardVector() * m_cameraMoveSpeed;
        break;
    case 2:
        m_firstPersonCameraPosition -= GetEyeSpaceRightVector() * m_cameraMoveSpeed;
        break;
    case 3:
        m_firstPersonCameraPosition += GetEyeSpaceRightVector() * m_cameraMoveSpeed;
        break;
    }
    SetTransformationDirty();
}

float FirstPersonCamera::GetCameraMoveSpeed() const
{
    return m_cameraMoveSpeed;
}

void FirstPersonCamera::Zoom(eMouseScrollDirection zoomDirection, float zoomPower)
{
    const auto forwardVector = m_eyeSpaceForwardVector;
    switch (zoomDirection) {
    case eMouseScrollDirection::ZoomIn: {
        m_firstPersonCameraPosition += forwardVector * zoomPower;
        break;
    }
    case eMouseScrollDirection::ZoomOut: {
        m_firstPersonCameraPosition -= forwardVector * zoomPower;
        break;
    }

    default:
        break;
    }

    SetTransformationDirty();
}
} // namespace EngineCore
