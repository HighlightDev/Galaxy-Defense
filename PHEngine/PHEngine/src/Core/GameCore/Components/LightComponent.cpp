#include "LightComponent.h"
#include "Core/GameCore/Scene.h"
#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/Components/ComponentData/LightComponentData.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"

using namespace Graphics::Renderer;

namespace EngineCore
{
   LightComponent::LightComponent(const std::shared_ptr<LightComponentData> &data)
       : SceneComponent(data->EngineObjectName, data->Translation, data->Rotation, data->Scale),
         mLightRenderData()
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

   void LightComponent::SetLightSceneProxyId(const int32_t lightSceneProxyId)
   {
      mLightSceneProxyId = lightSceneProxyId;
   }

   int32_t LightComponent::GetLightSceneProxyId() const
   {
      return mLightSceneProxyId;
   }

   eComponentType LightComponent::GetComponentType() const
   {
      return LIGHT_COMPONENT;
   }

   void LightComponent::UnpausableTick(const float deltaTime)
   {
      SceneComponent::UnpausableTick(deltaTime);

      if (bIsEnabledStateDirty ||
          bIsVisibleStateDirty)
      {
         SyncRenderData();
      }
   }

   void LightComponent::SetIsEnabled(const bool bEnabled)
   {
      if (mIsEnabled->GetValue() != bEnabled)
      {
         mIsEnabled->SetValue(bEnabled, false);
         bIsEnabledStateDirty = true;
         SyncRenderData();
      }
   }

   void LightComponent::SetIsVisible(const bool value)
   {
      if (mIsVisible != value)
      {
         mIsVisible = value;
         bIsVisibleStateDirty = true;
         SyncRenderData();
      }
   }

   bool LightComponent::IsVisible() const
   {
      return mIsVisible;
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

   void LightComponent::SyncRenderData()
   {
      if (bIsSceneProxyReady.load(std::memory_order::memory_order_seq_cst))
      {
         if (const auto &sceneSP = m_sceneWP.lock())
         {
            if (const auto &sceneRendererSp = sceneSP->GetInterThreadCommunicationManager().GetSceneRendererWP().lock())
            {
               if (bIsEnabledStateDirty)
               {
                  static const uint64_t functionId = Hash("LightComponent:UpdateLightComponentEnable_OnRenderThread");
                  sceneRendererSp->UpdateLightComponentEnable_OnRenderThread(mLightSceneProxyId, GetObjectId(), functionId, mIsEnabled->GetValue());
                  bIsEnabledStateDirty = false;
               }

               if (bIsVisibleStateDirty)
               {
                  static const uint64_t functionId = Hash("LightComponent:UpdateLightComponentIsVisible_OnRenderThread");
                  sceneRendererSp->UpdateLightComponentIsVisible_OnRenderThread(mLightSceneProxyId, GetObjectId(), functionId, mIsVisible);
                  bIsVisibleStateDirty = false;
               }
            }
         }
      }
   }

}