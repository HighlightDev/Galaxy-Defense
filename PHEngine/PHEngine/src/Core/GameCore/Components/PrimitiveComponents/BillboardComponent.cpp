#include "BillboardComponent.h"
#include "Core/GraphicsCore/SceneProxy/BillboardSceneProxy.h"
#include "Core/GameCore/Components/ComponentData/BillboardComponentData.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/GameCore/Scene.h"
#include "Core/CommonCore/StringHash.h"

namespace EngineCore
{

   BillboardComponent::BillboardComponent(const BillboardComponentData &data, const BillboardRenderData &renderData)
       : PrimitiveComponent(data.GameObjectName,
                            data.m_translation,
                            glm::vec3(),
                            data.m_scale,
                            renderData.m_skin->GetBoundingBox()),
         mBillboardExtent(data.m_billboardExtent),
         m_renderData(renderData)
   {
   }

   BillboardComponent::~BillboardComponent()
   {
   }

   eComponentType BillboardComponent::GetComponentType() const
   {
      return PRIMITIVE_COMPONENT;
   }

   void BillboardComponent::Tick(float deltaTime)
   {
   }

   void BillboardComponent::CollectDataForSerialization(SerializeDataContainer &dataContainer)
   {
      auto &actorData = GetSerializeDataActor(dataContainer);
   }

   std::shared_ptr<PrimitiveSceneProxy> BillboardComponent::CreateSceneProxy() const
   {
      return std::make_shared<BillboardSceneProxy>(this);
   }

   void BillboardComponent::SetBillboardExtent(const float extent)
   {
      mBillboardExtent = extent;

      if (const auto &sceneSp = m_sceneWP.lock())
      {
         if (const auto &sceneRenderer = sceneSp->GetThreadManager().TryGetSceneRendererWP().lock())
         {
            static const uint64_t functionId = Hash("BillboardComponent:SetBillboardExtent");

            sceneSp->ExecuteOnRenderThread(eEnqueueJobPolicy::IF_DUPLICATE_NO_PUSH, GetObjectId(), functionId, [=]()
                                           {
                                             const auto& billboardProxySp = std::static_pointer_cast<BillboardSceneProxy>(sceneRenderer->GetPrimitiveProxyByProxyId(SceneProxyId));
                                             assert(billboardProxySp);        
                                             billboardProxySp->SetBillboardExtent(extent); });
         }
      }
   }

   float BillboardComponent::GetBillboardExtent() const
   {
      return mBillboardExtent;
   }

   void BillboardComponent::SetBillboardTexture(const std::shared_ptr<ITexture> &texture)
   {
      if (const auto &sceneSp = m_sceneWP.lock())
      {
         if (const auto &sceneRenderer = sceneSp->GetThreadManager().TryGetSceneRendererWP().lock())
         {
            static const uint64_t functionId = Hash("BillboardComponent:SetBillboardTexture");

            sceneSp->ExecuteOnRenderThread(eEnqueueJobPolicy::IF_DUPLICATE_NO_PUSH, GetObjectId(), functionId, [=]()
                                           {
                                             const auto& billboardProxySp = std::static_pointer_cast<BillboardSceneProxy>(sceneRenderer->GetPrimitiveProxyByProxyId(SceneProxyId));
                                             assert(billboardProxySp);        
                                             billboardProxySp->SetBillboardTexture(texture); });
         }
      }
   }
}
