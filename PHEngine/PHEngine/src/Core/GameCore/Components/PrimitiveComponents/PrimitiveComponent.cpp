#include "PrimitiveComponent.h"
#include "Core/GameCore/Scene.h"
#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/BoundingBoxBuilder.h"

namespace Game
{

	PrimitiveComponent::PrimitiveComponent(const std::string& gameObjectName, glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale, BoundingBox boundingBox)
		: SceneComponent(gameObjectName, translation, rotation, scale)
      , mBoundingBox(boundingBox)
      , mIsVisible(GenericObjectProperty<bool>(true, "IsVisible"))
	{
      /******  HOOKS ****/
      ENGINE_PROPERTY("IsVisible", &mIsVisible);
      /******  HOOKS ****/
	}

	PrimitiveComponent::~PrimitiveComponent()
	{
	}

   ComponentType PrimitiveComponent::GetComponentType() const
   {
      return PRIMITIVE_COMPONENT;
   }

   void PrimitiveComponent::UpdateRelativeMatrix(glm::mat4& parentRelativeMatrix)
   {
      Base::UpdateRelativeMatrix(parentRelativeMatrix);

      // Update primitives proxy transform
      constexpr uint64_t functionId = Hash("PrimitiveComponent:UpdatePrimitiveComponentTransform_GameThread");

      m_scene->UpdatePrimitiveComponentTransform_GameThread(SceneProxyId, GetObjectId(), functionId, m_relativeMatrix, GetTransformedBoundingBox());
   }

   void PrimitiveComponent::SetIsEnabled(const bool bEnabled)
   {
      SceneComponent::SetIsEnabled(bEnabled);

      // Update primitives proxy enabled
      constexpr uint64_t functionId = Hash("PrimitiveComponent:UpdatePrimitiveComponentEnable_GameThread");
      m_scene->UpdatePrimitiveComponentEnable_GameThread(SceneProxyId, GetObjectId(), functionId, bEnabled);
   }

   void PrimitiveComponent::SetIsVisible(bool isVisible)
   {
      if (isVisible != mIsVisible)
      {
         mIsVisible = isVisible;
         OnVisibilityChanged();
      }
   }

   bool PrimitiveComponent::IsVisible() const 
   {
      return mIsVisible;
   }

   void PrimitiveComponent::OnVisibilityChanged()
   {
      constexpr uint64_t functionId = Hash("PrimitiveComponent::OnVisibilityChanged()");

      m_scene->UpdatePrimitiveComponentVisibility_GameThread(SceneProxyId, GetObjectId(), functionId, mIsVisible);
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