#include "PlanarReflectionProxy.h"
#include "Core/GameCore/Components/PlanarReflectionComponent.h"
#include "Core/UtilityCore/EngineMath.h"

using namespace EngineCore;

namespace Graphics {

   PlanarReflectionProxy::PlanarReflectionProxy(PlanarReflectionComponent const* planarReflectionComponent)
      : SceneProxyBase()
      , mRenderTargetViewPortInfo(planarReflectionComponent->GetRenderTargetViewPortInfo())
      , mReflectionPlane(planarReflectionComponent->GetReflectionPlane())
      , mPlanarReflectionFBO(std::make_unique<PlanarReflectionFramebuffer>(mRenderTargetViewPortInfo))
      , mMirrorMatrix(1)
   {
      mMirrorMatrix = EngineMath::BuildMirrorMatrix(mReflectionPlane);
   }

   PlanarReflectionProxy::~PlanarReflectionProxy()
   {
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
      mPlanarReflectionFBO->UnbindFramebuffer();
   }

   void PlanarReflectionProxy::ResolveReflectionRenderTargetSurfaceData() 
   {
      mPlanarReflectionFBO->ResolveReflectionRenderTargetSurfaceData();
   }

   void PlanarReflectionProxy::SetReflectionPlane(const glm::vec4& reflectionPlane) {
      mReflectionPlane = reflectionPlane;
      mMirrorMatrix = EngineMath::BuildMirrorMatrix(mReflectionPlane);
   }

   glm::mat4 PlanarReflectionProxy::GetMirrorMatrix() const {
      return mMirrorMatrix;
   }

   glm::vec4 PlanarReflectionProxy::GetReflectionPlane() const {
      return mReflectionPlane;
   }

   std::shared_ptr<ITexture> PlanarReflectionProxy::GetPlanarReflectionTexture() const {
      return mPlanarReflectionFBO->GetReflectionTexture();
   }

}
