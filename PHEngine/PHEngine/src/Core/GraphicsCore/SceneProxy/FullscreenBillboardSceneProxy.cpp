#include "FullscreenBillboardSceneProxy.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/GameCore/Scene.h"

#include <glm/mat4x4.hpp>

using namespace Graphics::Renderer;
using namespace EngineCore;
using namespace Resources;

namespace Graphics
{
   namespace Proxy
   {
      FullscreenBillboardSceneProxy::FullscreenBillboardSceneProxy(const FullscreenBillboardComponent *component)
          : BillboardSceneProxy(component)
      {
      }

      FullscreenBillboardSceneProxy::~FullscreenBillboardSceneProxy()
      {
      }

      void FullscreenBillboardSceneProxy::Render(const std::shared_ptr<CameraSceneProxy> &cameraSceneProxy, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix)
      {
         BillboardSceneProxy::Render(cameraSceneProxy, glm::mat4(1), glm::mat4(1));
      }

      bool FullscreenBillboardSceneProxy::CanBloomBeApplied() const
      {
         return false;
      }

   }
}
