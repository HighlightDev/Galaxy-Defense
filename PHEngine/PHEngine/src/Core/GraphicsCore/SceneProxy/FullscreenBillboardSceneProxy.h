#pragma once
#include "BillboardSceneProxy.h"
#include "Core/GameCore/Components/PrimitiveComponents/FullscreenBillboardComponent.h"

using namespace EngineCore;
using namespace EngineCore::ShaderImpl;
using namespace Graphics::Data;
using namespace Graphics::OpenGL;

namespace Graphics
{
   namespace Proxy
   {
      class FullscreenBillboardSceneProxy : public BillboardSceneProxy
      {
         using Base = BillboardSceneProxy;

      public:
         FullscreenBillboardSceneProxy(const FullscreenBillboardComponent *component);

         ~FullscreenBillboardSceneProxy() override;

         void Render(const std::shared_ptr<CameraSceneProxy> &cameraSceneProxy, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix) override;
      };

   }
}
