#include "SkyboxComponent.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/GraphicsCore/SceneProxy/SkyboxSceneProxy.h"

#include <glm/vec3.hpp>

namespace Game
{
	using namespace EngineUtility;

	SkyboxComponent::SkyboxComponent(glm::vec3 scale, float rotateSpeed, const SkyboxRenderData& renderData)
		: PrimitiveComponent(glm::vec3(), glm::vec3(), scale)
		, m_rotateSpeed(rotateSpeed)
      , m_renderData(renderData)
	{

	}

	SkyboxComponent::~SkyboxComponent()
	{

	}

	void SkyboxComponent::Tick(const float deltaTime)
	{
		Base::Tick(deltaTime);
		
		// SetRotationAxisY(m_rotation.y + deltaTime * m_rotateSpeed);
	}

   std::shared_ptr<PrimitiveSceneProxy> SkyboxComponent::CreateSceneProxy() const
   {
      return std::make_shared<SkyboxSceneProxy>(this);
   }

   uint64_t SkyboxComponent::GetComponentType() const
   {
      return SKYBOX_COMPONENT;
   }
}