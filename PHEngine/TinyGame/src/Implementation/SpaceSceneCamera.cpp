#include "SpaceSceneCamera.h"

#include "Core/GameCore/LoggerExtension.h"

namespace Game {
SpaceSceneCamera::SpaceSceneCamera(
    const std::string& cameraName,
    const eCameraType cameraType,
    const std::shared_ptr<Scene>& scene,
    const ViewPortInfo& viewPort,
    const std::shared_ptr<ViewProjectionInfo>& viewProjectionInfo,
    const float initPitchDeg,
    const float initYawDeg,
    glm::vec3 camPos)
    : FirstPersonCamera(cameraName, cameraType, scene, viewPort, viewProjectionInfo, initPitchDeg, initYawDeg, camPos)
    , mObservers()
{
}

void SpaceSceneCamera::Tick(const float deltaTime)
{
    ACamera::Tick(deltaTime);
}

void SpaceSceneCamera::OnTransformationUpdated()
{
    for (const auto& observer : mObservers) {
        observer->OnCameraTransformChanged(this);
    }
}

void SpaceSceneCamera::AddCameraTransformObserver(ICameraTransformChangeNotifyable* observer)
{
    mObservers.emplace_back(observer);
}
} // namespace Game