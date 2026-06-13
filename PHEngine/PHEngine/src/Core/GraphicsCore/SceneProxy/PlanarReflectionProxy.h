#pragma once

#include "Core/GameCore/FramebufferImplementation/PlanarReflectionFramebuffer.h"
#include "Core/GraphicsCore/SceneViewInfo/SceneView.h"
#include "SceneProxyBase.h"

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace EngineCore {
class PlanarReflectionComponent;
}

using namespace EngineCore::FramebufferImpl;

namespace Graphics {
using EngineCore::PlanarReflectionComponent;

class PlanarReflectionProxy : public SceneProxyBase {
    std::weak_ptr<SceneView> mCaptureSceneView;

    ViewPortInfo mRenderTargetViewPortInfo;

    glm::vec4 mReflectionPlane;

    glm::vec3 mReflectionPlaneOrigin;

    std::unique_ptr<PlanarReflectionFramebuffer> mPlanarReflectionFBO;

    glm::mat4 mMirrorMatrix;

public:
    PlanarReflectionProxy(PlanarReflectionComponent const* planarReflectionComponent);

    ~PlanarReflectionProxy();

    void CleanUp() override;

    void SetSceneViewWeakPtr(std::weak_ptr<SceneView> captureSceneView);

    std::weak_ptr<SceneView> GetSceneViewWeakPtr() const;

    void RenderToPlanarReflectionFBO();

    void StopRenderingToPlanarReflectionFBO();

    void ResolveReflectionRenderTargetSurfaceData();

    void SetReflectionPlane(const glm::vec4& reflectionPlane);

    void SetReflectionPlaneOrigin(const glm::vec3& reflectionPlaneOrigin);

    const glm::vec4& GetReflectionPlane() const;

    const glm::vec3& GetReflectionPlaneOrigin() const;

    const glm::mat4& GetMirrorMatrix() const;

    std::shared_ptr<ITexture> GetPlanarReflectionTexture() const;
};
} // namespace Graphics
