#include "LightComponent.h"
#include "Core/GameCore/Scene.h"
#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/Components/ComponentData/LightComponentData.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"

using namespace Graphics::Renderer;

namespace EngineCore
{
   LightComponent::LightComponent(const std::shared_ptr<LightComponentData> &data)
       : SceneComponent(data->EngineObjectName, data->Translation, data->Rotation, data->Scale), mLightRenderData()
   {
   }

   LightComponent::~LightComponent()
   {
   }

   void LightComponent::SetIsSceneProxyReady(const bool isSceneProxyReady)
   {
      bIsSceneProxyReady.store(isSceneProxyReady, std::memory_order::memory_order_seq_cst);
   }

   bool LightComponent::IsSceneProxyReady() const
   {
      return bIsSceneProxyReady.load(std::memory_order::memory_order_seq_cst);
   }

   void LightComponent::SetLightSceneProxyId(const size_t lightSceneProxyId)
   {
      mLightSceneProxyId = lightSceneProxyId;
   }

   size_t LightComponent::GetLightSceneProxyId() const
   {
      return mLightSceneProxyId;
   }

   eComponentType LightComponent::GetComponentType() const
   {
      return LIGHT_COMPONENT;
   }

   void LightComponent::UpdateRelativeMatrix(const glm::mat4 &parentRelativeMatrix)
   {
      Base::UpdateRelativeMatrix(parentRelativeMatrix);
      // Update light proxy transform
      static const uint64_t functionId = Hash("LightComponent::UpdateLightComponentTransform_GameThread");

      if (bIsSceneProxyReady.load(std::memory_order::memory_order_seq_cst))
      {
         SetIsTransformationDirty(false);
         if (const auto &sceneSP = m_sceneWP.lock())
         {
            if (const auto &sceneRendererSp = sceneSP->GetInterThreadCommunicationManager().GetSceneRendererWP().lock())
            {
               sceneRendererSp->UpdateLightComponentTransform_OnRenderThread(mLightSceneProxyId, GetObjectId(), functionId, m_relativeMatrix);
            }
         }
      }
      else
      {
         SetIsTransformationDirty(true);
      }
   }

}