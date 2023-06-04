#include "BillboardComponent.h"
#include "Core/GraphicsCore/SceneProxy/BillboardSceneProxy.h"
#include "Core/GameCore/Components/ComponentData/BillboardComponentData.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/GameCore/Scene.h"
#include "Core/CommonCore/StringHash.h"

namespace EngineCore
{

   BillboardComponent::BillboardComponent(const BillboardComponentData &data, const BillboardRenderData &renderData)
       : PrimitiveComponent(data.EngineObjectName,
                            data.m_translation,
                            glm::vec3(),
                            data.m_scale),
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

   void BillboardComponent::UnpausableTick(float deltaTime)
   {
      PrimitiveComponent::UnpausableTick(deltaTime);

      if (bIsExtentDataDirty && bIsSceneProxyReady.load(std::memory_order::memory_order_seq_cst))
      {
         SyncRenderData();
         bIsExtentDataDirty = false;
      }
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
      if (mBillboardExtent != extent)
      {
         mBillboardExtent = extent;
         bIsExtentDataDirty = true;
      }
   }

   float BillboardComponent::GetBillboardExtent() const
   {
      return mBillboardExtent;
   }

   void BillboardComponent::SetIsEnabled(const bool bEnabled)
   {
      PrimitiveComponent::SetIsEnabled(bEnabled);

      const auto &material = GetMaterial();
      if (IMaterial::eMaterialType::DYNAMIC == material->GetMaterialType())
      {
         material->SetIsEnabled(bEnabled);
      }
   }

   void BillboardComponent::SetIsVisible(bool isVisible)
   {
      PrimitiveComponent::SetIsVisible(isVisible);

      const auto &material = GetMaterial();
      if (IMaterial::eMaterialType::DYNAMIC == material->GetMaterialType())
      {
         material->SetIsEnabled(isVisible);
      }
   }

   std::shared_ptr<IMaterial> BillboardComponent::GetMaterial() const
   {
      std::shared_ptr<IMaterial> materialResult = nullptr;
      if (const auto &sceneSP = m_sceneWP.lock())
      {
         materialResult = sceneSP->GetMaterialByProxyId(m_renderData.mMaterialProxy->GetSceneProxyId());
      }
      assert(materialResult != nullptr);
      return materialResult;
   }

   void BillboardComponent::SyncRenderData()
   {
      if (const auto &sceneSp = m_sceneWP.lock())
      {
         if (const auto &sceneRenderer = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock())
         {
            static const uint64_t functionId = Hash("BillboardComponent:SetBillboardExtent");
            sceneSp->GetInterThreadCommunicationManager().ExecuteOnRenderThread(eEnqueueJobPolicy::IF_DUPLICATE_NO_PUSH, GetObjectId(), functionId, [sceneRenderer, sceneProxyId = mSceneProxyId, billboardExtent = mBillboardExtent]()
                                                                                {
               if (const auto &billboardProxySp = std::static_pointer_cast<BillboardSceneProxy>(sceneRenderer->GetPrimitiveProxyByProxyId(sceneProxyId)))
               {
                  billboardProxySp->SetBillboardExtent(billboardExtent);
               } });
         }
      }
   }
}
