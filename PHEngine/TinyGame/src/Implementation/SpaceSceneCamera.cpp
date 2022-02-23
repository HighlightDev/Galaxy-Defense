#include "SpaceSceneCamera.h"

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
                            camPos)
    {
    }

    void SpaceSceneCamera::Tick(const float deltaTime)
    {
        ACamera::Tick(deltaTime);
    }
}