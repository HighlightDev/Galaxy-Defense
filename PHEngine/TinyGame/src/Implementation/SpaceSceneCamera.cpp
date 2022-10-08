#include "SpaceSceneCamera.h"

#include "Core/GameCore/LoggerExtension.h"

namespace Game
{
    SpaceSceneCamera::SpaceSceneCamera(const std::string &cameraName, const eCameraType cameraType,
                                       std::shared_ptr<Scene> scene, const ViewPortInfo &viewPort, const float initPitchDeg,
                                       const float initYawDeg, glm::vec3 camPos)
        : FirstPersonCamera(cameraName,
                            cameraType,
                            scene,
                            viewPort,
                            initPitchDeg,
                            initYawDeg,
                            camPos),
          mObservers()
    {
    }

    void SpaceSceneCamera::Tick(const float deltaTime)
    {
        ACamera::Tick(deltaTime);
    }

    void SpaceSceneCamera::OnTransformationUpdated()
    {
        LogInfo("SpaceSceneCamera::OnTransformationUpdated");
        for (const auto &observer : mObservers)
        {
            LogInfo("observer->OnCameraTransformChanged");
            observer->OnCameraTransformChanged(this);
        }
    }

    void SpaceSceneCamera::AddCameraTransformObserver(ICameraTransformChangeNotifyable *observer)
    {
        LogInfo("SpaceSceneCamera::AddCameraTransformObserver");
        mObservers.emplace_back(observer);
    }
}