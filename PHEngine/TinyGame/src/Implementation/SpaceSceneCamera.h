#pragma once

#include "Core/GameCore/FirstPersonCamera.h"
#include "ICameraTransformChangeNotifyable.h"

#include <vector>

namespace Game
{
    class SpaceSceneCamera : public FirstPersonCamera
    {

        std::vector<ICameraTransformChangeNotifyable*> mObservers;

    public:
        SpaceSceneCamera(const std::string &cameraName, const eCameraType cameraType,
                         std::shared_ptr<Scene> scene, const ViewPortInfo &viewPort, const float initPitchDeg,
                         const float initYawDeg, glm::vec3 camPos);

        virtual void Tick(const float deltaTime) override;

        void AddCameraTransformObserver(ICameraTransformChangeNotifyable* observer);

    protected:
        virtual void OnTransformationUpdated() override;
    };
}