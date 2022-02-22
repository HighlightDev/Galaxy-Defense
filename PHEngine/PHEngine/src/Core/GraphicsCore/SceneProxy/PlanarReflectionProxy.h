#pragma once

#include "SceneProxyBase.h"
#include "Core/GraphicsCore/SceneViewInfo/SceneView.h"
#include "Core/GameCore/FramebufferImplementation/PlanarReflectionFramebuffer.h"

namespace EngineCore {
   class PlanarReflectionComponent;
}

using namespace EngineCore::FramebufferImpl;

namespace Graphics
{
   using EngineCore::PlanarReflectionComponent;

   class PlanarReflectionProxy
      : public SceneProxyBase
   {
      std::weak_ptr<SceneView> mCaptureSceneView;

      ViewPortInfo mRenderTargetViewPortInfo;

      glm::vec4 mReflectionPlane;

      std::unique_ptr<PlanarReflectionFramebuffer> mPlanarReflectionFBO;
      
      glm::mat4 mMirrorMatrix;

   public:

      PlanarReflectionProxy(PlanarReflectionComponent const* planarReflectionComponent);

      ~PlanarReflectionProxy();

      void SetSceneViewWeakPtr(std::weak_ptr<SceneView> captureSceneView);

      std::weak_ptr<SceneView> GetSceneViewWeakPtr() const;

      void RenderToPlanarReflectionFBO();

      void StopRenderingToPlanarReflectionFBO();

      void ResolveReflectionRenderTargetSurfaceData();

      void SetReflectionPlane(const glm::vec4& reflectionPlane);

      glm::vec4 GetReflectionPlane() const;

      glm::mat4 GetMirrorMatrix() const;

      std::shared_ptr<ITexture> GetPlanarReflectionTexture() const;

   };
}

