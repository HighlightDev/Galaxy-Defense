#include "PrimitiveComponent.h"
#include "Core/GameCore/Scene.h"
#include "Core/CommonCore/StringHash.h"

namespace Game
{

   size_t PrimitiveComponent::TotalPrimitiveSceneProxyIndex = 0;

	PrimitiveComponent::PrimitiveComponent(const std::string& gameObjectName, glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale)
		: SceneComponent(gameObjectName, translation, rotation, scale)
	{
	}

	PrimitiveComponent::~PrimitiveComponent()
	{
	}

   uint64_t PrimitiveComponent::GetComponentType() const
   {
      return PRIMITIVE_COMPONENT;
   }

   void PrimitiveComponent::UpdateRelativeMatrix(glm::mat4& parentRelativeMatrix)
   {
      Base::UpdateRelativeMatrix(parentRelativeMatrix);

      // Update primitives proxy transform
      constexpr uint64_t functionId = Hash("PrimitiveComponent:OnUpdatePrimitiveComponentTransform_GameThread");

      m_scene->OnUpdatePrimitiveComponentTransform_GameThread(PrimitiveProxyComponentId, GetObjectId(), functionId, m_relativeMatrix);
   }

   void PrimitiveComponent::OnVisibilityChanged()
   {
      constexpr uint64_t functionId = Hash("PrimitiveComponent::OnVisibilityChanged()");

      m_scene->OnUpdatePrimitiveComponentVisibility_GameThread(PrimitiveProxyComponentId, GetObjectId(), functionId, mIsVisible);
   }
}