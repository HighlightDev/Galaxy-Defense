#include "PrimitiveComponent.h"
#include "Core/GameCore/Scene.h"
#include "Core/CommonCore/StringHash.h"

namespace Game
{

	PrimitiveComponent::PrimitiveComponent(const std::string& gameObjectName, glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale)
		: SceneComponent(gameObjectName, translation, rotation, scale)
	{
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

      m_scene->UpdatePrimitiveComponentTransform_GameThread(SceneProxyId, GetObjectId(), functionId, m_relativeMatrix);
   }

   void PrimitiveComponent::OnVisibilityChanged()
   {
      constexpr uint64_t functionId = Hash("PrimitiveComponent::OnVisibilityChanged()");

      m_scene->UpdatePrimitiveComponentVisibility_GameThread(SceneProxyId, GetObjectId(), functionId, mIsVisible);
   }
}