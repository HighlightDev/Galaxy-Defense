#include "PlanarReflectionProxy.h"

#include "Core/GameCore/Components/PlanarReflectionComponent.h"
#include "Core/UtilityCore/EngineMath.h"

using namespace EngineCore;

namespace Graphics {

PlanarReflectionProxy::PlanarReflectionProxy(PlanarReflectionComponent const* planarReflectionComponent)
    : SceneProxyBase(planarReflectionComponent->IsEnabled())
    , mRenderTargetViewPortInfo(planarReflectionComponent->GetRenderTargetViewPortInfo())
    , mReflectionPlane(planarReflectionComponent->GetReflectionPlane())
    , mReflectionPlaneOrigin(planarReflectionComponent->GetReflectionPlaneOrigin())
    , mPlanarReflectionFBO(std::make_unique<PlanarReflectionFramebuffer>(mRenderTargetViewPortInfo))
    , mMirrorMatrix(1)
{
    mMirrorMatrix = EngineMath::BuildMirrorMatrix(mReflectionPlane);
}

PlanarReflectionProxy::~PlanarReflectionProxy()
{
}

void PlanarReflectionProxy::CleanUp()
{
    mPlanarReflectionFBO->CleanUp();
}

void PlanarReflectionProxy::SetSceneViewWeakPtr(std::weak_ptr<SceneView> captureSceneView)
{
    mCaptureSceneView = captureSceneView;
}

std::weak_ptr<SceneView> PlanarReflectionProxy::GetSceneViewWeakPtr() const
{
    return mCaptureSceneView;
}

void PlanarReflectionProxy::RenderToPlanarReflectionFBO()
{
    mPlanarReflectionFBO->RenderToTexture();
}

void PlanarReflectionProxy::StopRenderingToPlanarReflectionFBO()
{
    mPlanarReflectionFBO->UnbindFramebuffer(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

void PlanarReflectionProxy::ResolveReflectionRenderTargetSurfaceData()
{
    mPlanarReflectionFBO->ResolveReflectionRenderTargetSurfaceData();
}

void PlanarReflectionProxy::SetReflectionPlane(const glm::vec4& reflectionPlane)
{
    mReflectionPlane = reflectionPlane;
    mMirrorMatrix = EngineMath::BuildMirrorMatrix(mReflectionPlane);
}

const glm::mat4& PlanarReflectionProxy::GetMirrorMatrix() const
{
    return mMirrorMatrix;
}

const glm::vec4& PlanarReflectionProxy::GetReflectionPlane() const
{
    return mReflectionPlane;
}

const glm::vec3& PlanarReflectionProxy::GetReflectionPlaneOrigin() const
{
    return mReflectionPlaneOrigin;
}

void PlanarReflectionProxy::SetReflectionPlaneOrigin(const glm::vec3& reflectionPlaneOrigin)
{
    mReflectionPlaneOrigin = reflectionPlaneOrigin;
}

std::shared_ptr<ITexture> PlanarReflectionProxy::GetPlanarReflectionTexture() const
{
    return mPlanarReflectionFBO->GetReflectionTexture();
}

} // namespace Graphics
