#include "StaticMeshComponent.h"
#include "Core/GraphicsCore/SceneProxy/StaticMeshSceneProxy.h"

#include <memory>

namespace Game
{

	StaticMeshComponent::StaticMeshComponent(const std::string& gameObjectName, glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale, const StaticMeshRenderData& renderData)
		: PrimitiveComponent(gameObjectName, std::move(translation), std::move(rotation), std::move(scale))
      , m_renderData(renderData)
	{

	}

	StaticMeshComponent::~StaticMeshComponent()
	{

	}

	void StaticMeshComponent::Tick(const float deltaTime)
	{
		Base::Tick(deltaTime);
	}

   std::shared_ptr<PrimitiveSceneProxy> StaticMeshComponent::CreateSceneProxy() const
   {
      return std::make_shared<StaticMeshSceneProxy>(this);
   }

   ComponentType StaticMeshComponent::GetComponentType() const
   {
      return PRIMITIVE_COMPONENT;
   }

}
