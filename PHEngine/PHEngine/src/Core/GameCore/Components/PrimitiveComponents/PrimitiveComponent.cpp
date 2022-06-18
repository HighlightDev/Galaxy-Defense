#include "PrimitiveComponent.h"
#include "Core/GameCore/Scene.h"
#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/BoundingBoxBuilder.h"

namespace EngineCore
{

   PrimitiveComponent::PrimitiveComponent(const std::string &gameObjectName,
                                          const glm::vec3 &translation,
                                          const glm::vec3 &rotation,
                                          const glm::vec3 &scale,
                                          const BoundingBox &boundingBox)
       : SceneComponent(gameObjectName,
                        translation,
                        rotation,
                        scale),
         mBoundingBox(boundingBox),
         mIsVisible(std::make_shared<EngineGOProperty<bool>>(true, "IsVisible"))
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

   void PrimitiveComponent::UpdateRelativeMatrix(const glm::mat4 &parentRelativeMatrix)
   {
      Base::UpdateRelativeMatrix(parentRelativeMatrix);

      // Update primitives proxy transform
      static const uint64_t functionId = Hash("PrimitiveComponent:UpdatePrimitiveComponentTransform_GameThread");

      if (const auto &sceneSP = m_sceneWP.lock())
      {
         sceneSP->UpdatePrimitiveComponentTransform_OnRenderThread(SceneProxyId, GetObjectId(), functionId, m_relativeMatrix, GetTransformedBoundingBox());
      }
   }

   void PrimitiveComponent::SetIsEnabled(const bool bEnabled)
   {
      SceneComponent::SetIsEnabled(bEnabled);

      // Update primitives proxy enabled
      static const uint64_t functionId = Hash("PrimitiveComponent:UpdatePrimitiveComponentEnable_GameThread");
      if (const auto &sceneSP = m_sceneWP.lock())
      {
         sceneSP->UpdatePrimitiveComponentEnable_OnRenderThread(SceneProxyId, GetObjectId(), functionId, bEnabled);
      }
   }

   void PrimitiveComponent::SetIsVisible(bool isVisible)
   {
      if (isVisible != mIsVisible->GetValue())
      {
         mIsVisible->SetValue(isVisible);
         OnVisibilityChanged();
      }
   }

   bool PrimitiveComponent::IsVisible() const
   {
      return mIsVisible->GetValue();
   }

   void PrimitiveComponent::OnVisibilityChanged()
   {
      static const uint64_t functionId = Hash("PrimitiveComponent::OnVisibilityChanged()");

      if (const auto &sceneSP = m_sceneWP.lock())
      {
         sceneSP->UpdatePrimitiveComponentVisibility_OnRenderThread(SceneProxyId, GetObjectId(), functionId, mIsVisible->GetValue());
      }
   }

   BoundingBox PrimitiveComponent::GetBoundingBox() const
   {
      return mBoundingBox;
   }

   BoundingBox PrimitiveComponent::GetTransformedBoundingBox() const
   {
      return BoundingBoxBuilder::GetTransformedBoundingBox(mBoundingBox, m_relativeMatrix);
   }
}