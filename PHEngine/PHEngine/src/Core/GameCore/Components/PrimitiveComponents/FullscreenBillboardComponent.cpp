#include "FullscreenBillboardComponent.h"
#include "Core/GraphicsCore/SceneProxy/FullscreenBillboardSceneProxy.h"
#include "Core/GameCore/Components/ComponentData/BillboardComponentData.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/GameCore/Scene.h"
#include "Core/CommonCore/StringHash.h"

namespace EngineCore
{

   FullscreenBillboardComponent::FullscreenBillboardComponent(const std::shared_ptr<BillboardComponentData> &data, const BillboardRenderData &renderData)
       : BillboardComponent(data, renderData)
   {
   }

   FullscreenBillboardComponent::~FullscreenBillboardComponent()
   {
   }

   std::shared_ptr<PrimitiveSceneProxy> FullscreenBillboardComponent::CreateSceneProxy() const
   {
      return std::make_shared<FullscreenBillboardSceneProxy>(this);
   }
}
