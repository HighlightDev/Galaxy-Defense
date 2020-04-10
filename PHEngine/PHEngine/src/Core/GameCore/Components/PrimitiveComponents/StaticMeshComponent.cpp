#include "StaticMeshComponent.h"
#include "Core/GraphicsCore/SceneProxy/StaticMeshSceneProxy.h"

#include <memory>

namespace Game
{

	StaticMeshComponent::StaticMeshComponent(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale, const StaticMeshRenderData& renderData)
		: PrimitiveComponent(std::move(translation), std::move(rotation), std::move(scale))
      , m_renderData(renderData)
	{

	}

	StaticMeshComponent::~StaticMeshComponent()
	{

	}

	void StaticMeshComponent::Tick(const float deltaTime)
	{
		Base::Tick(deltaTime);

		/*m_eulerRotationDegrees += 0.02f;

		if (m_eulerRotationDegrees.x > 360.0f)
			m_eulerRotationDegrees.x -= 360.0f;

		if (m_eulerRotationDegrees.y > 360.0f)
			m_eulerRotationDegrees.y -= 360.0f;

		if (m_eulerRotationDegrees.z > 360.0f)
			m_eulerRotationDegrees.z -= 360.0f;*/
	}

   std::shared_ptr<PrimitiveSceneProxy> StaticMeshComponent::CreateSceneProxy() const
   {
      return std::make_shared<StaticMeshSceneProxy>(this);
   }

   uint64_t StaticMeshComponent::GetComponentType() const
   {
      return STATIC_MESH_COMPONENT;
   }

}
