#pragma once

#include "SceneProxyBase.h"
#include "Core/GraphicsCore/SceneViewInfo/SceneView.h"
#include "Core/GameCore/FramebufferImplementation/PlanarReflectionFramebuffer.h"

namespace Game {
   class PlanarReflectionComponent;
}

using namespace Game::FramebufferImpl;

namespace Graphics
{
   using Game::PlanarReflectionComponent;

   class PlanarReflectionProxy
      : public SceneProxyBase
   {
      std::weak_ptr<SceneView> mCaptureSceneView;

      ViewPortInfo mRenderTargetViewPortInfo;

      std::unique_ptr<PlanarReflectionFramebuffer> mPlanarReflectionFBO;

   public:

      PlanarReflectionProxy(PlanarReflectionComponent const* planarReflectionComponent);

      ~PlanarReflectionProxy();

      void SetSceneViewWeakPtr(std::weak_ptr<SceneView> captureSceneView);
   };
}

