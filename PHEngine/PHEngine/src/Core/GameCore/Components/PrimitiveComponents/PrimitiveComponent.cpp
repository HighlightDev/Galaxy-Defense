#include "PrimitiveComponent.h"
#include "Core/GameCore/Scene.h"
#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/BoundingBoxBuilder.h"

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
         mIsVisible(std::make_shared<EngineGOProperty<bool>>(true, "p_isVisible"))
   {
      /******  HOOKS ****/
      AddEngineProperty(mIsVisible);
      /******  HOOKS ****/
   }

   PrimitiveComponent::~PrimitiveComponent()
   {
   }

   eComponentType PrimitiveComponent::GetComponentType() const
   {
      return PRIMITIVE_COMPONENT;
   }

   void PrimitiveComponent::Tick(const float deltaTime)
   {
      SceneComponent::Tick(deltaTime);

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

      // Update primitives proxy transform
      static const uint64_t functionId = Hash("PrimitiveComponent:UpdatePrimitiveComponentTransform_GameThread");

      if (const auto &sceneSP = m_sceneWP.lock())
      {
         const auto updateSuccessfull = sceneSP->UpdatePrimitiveComponentTransform_OnRenderThread(SceneProxyId, GetObjectId(), functionId, m_relativeMatrix, GetTransformedBoundingBox());
         SetIsTransformationDirty(!updateSuccessfull);
      }
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

   BoundingBox3D PrimitiveComponent::GetTransformedBoundingBox() const
   {
      return BoundingBoxBuilder::GetTransformedBoundingBox(mBoundingBox, m_relativeMatrix);
   }

   void PrimitiveComponent::SetBoundingBox(const BoundingBox3D &boundingBox)
   {
      mBoundingBox = boundingBox;
   }

   void PrimitiveComponent::SyncRenderData()
   {
      if (const auto &sceneSP = m_sceneWP.lock())
      {
         if (bIsEnabledStateDirty)
         {
            static const uint64_t functionId = Hash("PrimitiveComponent:UpdatePrimitiveComponentEnable_GameThread");
            const auto updateSuccessfull = sceneSP->UpdatePrimitiveComponentEnable_OnRenderThread(SceneProxyId, GetObjectId(), functionId, mIsEnabled->GetValue());
            bIsEnabledStateDirty = !updateSuccessfull;
         }

         if (bIsVisibleStateDirty)
         {
            static const uint64_t functionId = Hash("PrimitiveComponent::UpdatePrimitiveComponentVisibility_OnRenderThread()");
            const auto updateSuccessfull = sceneSP->UpdatePrimitiveComponentVisibility_OnRenderThread(SceneProxyId, GetObjectId(), functionId, mIsVisible->GetValue());
            bIsVisibleStateDirty = !updateSuccessfull;
         }

         if (bIsSortOrderStateDirty)
         {
            static constexpr uint64_t functionId = Hash64_CT("PrimitiveComponent::UpdatePrimitiveComponentSortOrderValue_OnRenderThread()");
            const auto updateSuccessfull = sceneSP->UpdatePrimitiveComponentSortOrderValue_OnRenderThread(SceneProxyId, GetObjectId(), functionId, mSortOrderValue);
            bIsSortOrderStateDirty = !updateSuccessfull;
         }
      }
   }
}