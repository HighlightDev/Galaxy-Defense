#include "PlanarReflectionProxy.h"
#include "Core/GameCore/Components/PlanarReflectionComponent.h"

using namespace Game;

namespace Graphics {

   PlanarReflectionProxy::PlanarReflectionProxy(PlanarReflectionComponent const* planarReflectionComponent)
      : SceneProxyBase()
      , mRenderTargetViewPortInfo(planarReflectionComponent->GetRenderTargetViewPortInfo())
      , mPlanarReflectionFBO(std::make_unique<PlanarReflectionFramebuffer>(mRenderTargetViewPortInfo))
   {
   }

   PlanarReflectionProxy::~PlanarReflectionProxy()
   {
   }

   void PlanarReflectionProxy::SetSceneViewWeakPtr(std::weak_ptr<SceneView> captureSceneView)
   {
      mCaptureSceneView = captureSceneView;
   }
}
