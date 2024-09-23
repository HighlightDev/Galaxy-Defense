#include "PrimitiveComponent.h"
#include "Core/GameCore/Scene.h"
#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/BoundingBoxBuilder.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"

using namespace Graphics::Renderer;

namespace EngineCore
{

   PrimitiveComponent::PrimitiveComponent(const std::string &gameObjectName,
                                          const glm::vec3 &translation,
                                          const glm::vec3 &rotation,
                                          const glm::vec3 &scale)
       : SceneComponent(gameObjectName,
                        translation,
                        rotation,
                        scale),
         mBoundingBox(BoundingBox3D()),
         mIsVisible(std::make_shared<EngineObjectProperty<bool>>(true, "p_isVisible"))
   {
      /******  HOOKS ****/
      AddEngineProperty(mIsVisible);
      /******  HOOKS ****/
   }

   PrimitiveComponent::~PrimitiveComponent()
   {
   }

   void PrimitiveComponent::SetSceneProxyId(const int32_t proxyId)
   {
      mSceneProxyId = proxyId;
   }

   int32_t PrimitiveComponent::GetSceneProxyId() const
   {
      return mSceneProxyId;
   }

   eComponentType PrimitiveComponent::GetComponentType() const
   {
      return PRIMITIVE_COMPONENT;
   }

   void PrimitiveComponent::UnpausableTick(const float deltaTime)
   {
      SceneComponent::UnpausableTick(deltaTime);

      if (bIsEnabledStateDirty ||
          bIsVisibleStateDirty ||
          bIsSortOrderStateDirty)
      {
         SyncRenderData();
      }
   }

   void PrimitiveComponent::UpdateRelativeMatrix(const glm::mat4 &parentRelativeMatrix)
   {
      Base::UpdateRelativeMatrix(parentRelativeMatrix);

      if (bIsSceneProxyReady.load(std::memory_order::memory_order_seq_cst))
      {
         // Update primitives proxy transform
         static const uint64_t functionId = Hash("PrimitiveComponent:UpdatePrimitiveComponentTransform_GameThread");

         if (const auto &sceneSp = m_sceneWP.lock())
         {
            if (const auto &sceneRendererSp = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock())
            {
               sceneRendererSp->UpdatePrimitiveComponentTransform_OnRenderThread(mSceneProxyId, GetObjectId(), functionId, m_relativeMatrix, GetTransformedBoundingBox());
            }
         }
      }
      SetIsTransformationDirty(!bIsSceneProxyReady);
   }

   void PrimitiveComponent::SetIsEnabled(const bool bEnabled)
   {
      if (mIsEnabled->GetValue() != bEnabled)
      {
         mIsEnabled->SetValue(bEnabled, false);
         bIsEnabledStateDirty = true;
         SyncRenderData();
      }
   }

   void PrimitiveComponent::SetIsVisible(bool isVisible)
   {
      if (isVisible != mIsVisible->GetValue())
      {
         mIsVisible->SetValue(isVisible);
         bIsVisibleStateDirty = true;
         SyncRenderData();
      }
   }

   bool PrimitiveComponent::IsVisible() const
   {
      return mIsVisible->GetValue();
   }

   BoundingBox3D PrimitiveComponent::GetBoundingBox() const
   {
      return mBoundingBox;
   }

   void PrimitiveComponent::SetSortOrderValue(const int32_t orderValue)
   {
      if (mSortOrderValue != orderValue)
      {
         mSortOrderValue = orderValue;
         bIsSortOrderStateDirty = true;
         SyncRenderData();
      }
   }

   int32_t PrimitiveComponent::GetSortOrderValue() const
   {
      return mSortOrderValue;
   }

   void PrimitiveComponent::SetIsSceneProxyReady(const bool isReady)
   {
      bIsSceneProxyReady.store(isReady, std::memory_order::memory_order_seq_cst);
   }

   bool PrimitiveComponent::IsSceneProxyReady() const
   {
      return bIsSceneProxyReady.load(std::memory_order::memory_order_seq_cst);
   }

   BoundingBox3D PrimitiveComponent::GetTransformedBoundingBox() const
   {
      return BoundingBoxBuilder::GetTransformedBoundingBox(mBoundingBox, m_relativeMatrix);
   }

   void PrimitiveComponent::SetBoundingBox(const BoundingBox3D &boundingBox)
   {
      mBoundingBox = boundingBox;
      SetIsTransformationDirty(true); // Update transform for bounding box and sync with render thread
   }

   void PrimitiveComponent::SyncRenderData()
   {
      if (bIsSceneProxyReady.load(std::memory_order::memory_order_seq_cst))
      {
         if (const auto &sceneSP = m_sceneWP.lock())
         {
            if (const auto &sceneRendererSp = sceneSP->GetInterThreadCommunicationManager().GetSceneRendererWP().lock())
            {
               if (bIsEnabledStateDirty)
               {
                  static const uint64_t functionId = Hash("PrimitiveComponent:UpdatePrimitiveComponentEnable_GameThread");
                  sceneRendererSp->UpdatePrimitiveComponentEnable_OnRenderThread(mSceneProxyId, GetObjectId(), functionId, mIsEnabled->GetValue());
                  bIsEnabledStateDirty = false;
               }

               if (bIsVisibleStateDirty)
               {
                  static const uint64_t functionId = Hash("PrimitiveComponent::UpdatePrimitiveComponentVisibility_OnRenderThread()");
                  sceneRendererSp->UpdatePrimitiveComponentVisibility_OnRenderThread(mSceneProxyId, GetObjectId(), functionId, mIsVisible->GetValue());
                  bIsVisibleStateDirty = false;
               }

               if (bIsSortOrderStateDirty)
               {
                  static constexpr uint64_t functionId = Hash64_CT("PrimitiveComponent::UpdatePrimitiveComponentSortOrderValue_OnRenderThread()");
                  sceneRendererSp->UpdatePrimitiveComponentSortOrderValue_OnRenderThread(mSceneProxyId, GetObjectId(), functionId, mSortOrderValue);
                  bIsSortOrderStateDirty = false;
               }
            }
         }
      }
   }
}