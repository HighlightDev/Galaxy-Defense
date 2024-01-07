#include "GalaxySceneCamera.h"

#include "Core/GameCore/LoggerExtension.h"

namespace Game
{
    GalaxySceneCamera::GalaxySceneCamera(const std::string &cameraName,
                                         const eCameraType cameraType,
                                         std::shared_ptr<Scene> scene,
                                         const ViewPortInfo &viewPort,
                                         const std::shared_ptr<ViewProjectionInfo> &viewProjectionInfo,
                                         const float initPitchDeg,
                                         const float initYawDeg,
                                         const float camDistanceToThirdPersonTarget,
                                         const glm::vec3 &thirdPersonTargetOffset)
        : ThirdPersonCamera(cameraName,
                            cameraType,
                            scene,
                            viewPort,
                            viewProjectionInfo,
                            initPitchDeg,
                            initYawDeg,
                            camDistanceToThirdPersonTarget,
                            thirdPersonTargetOffset)
    {
    }

    void GalaxySceneCamera::Tick(const float deltaTime)
    {
        ACamera::Tick(deltaTime);

        const auto &mouseBindings = mInputComponent->GetMouseBindings();

        if (mouseBindings->IsMouseScrollEventDirty())
        {
            const auto mouseZoomDirection = mouseBindings->FlushMouseScrollEvent();
            Zoom(mouseZoomDirection, 5.0f);
        }

        if (m_bThirdPersonTargetTransformationDirty)
        {
            const float clampedDeltaTime = std::max(deltaTime, 0.03f);
            m_lerpTimeElapsed = std::min(m_lerpTimeElapsed + clampedDeltaTime, m_timeForInterpolation);

            glm::vec3 finalTargetVector = m_thirdPersonTarget->GetRootComponent()->GetTranslation();
            m_actualTargetVector = EngineMath::LerpVec3(m_lerpTimeElapsed, 0.0f, m_timeForInterpolation, m_actualTargetVector, finalTargetVector);

            SetTransformationDirty();

            // If camera is at final position
            if (EngineMath::FloatsNearEqual(m_lerpTimeElapsed, m_timeForInterpolation))
            {
                m_lerpTimeElapsed = 0.0f;
                m_bThirdPersonTargetTransformationDirty = false;
            }
        }
    }
}